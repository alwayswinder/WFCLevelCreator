// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelCreatorLibrary.h"

#include "DataTableEditorUtils.h"
#include "IImageWrapper.h"
#include "ObjectTools.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "IImageWrapperModule.h"
#include "ImageCoreUtils.h"


bool ULevelCreatorLibrary::Generic_AddRowDT(void* Target, FProperty* Pro, UDataTable* DT, FName NewRowName)
{
	if (!Target || !Pro || !DT)
	{
		return false;
	}
	FStructProperty* StructPro = CastField<FStructProperty>(Pro);
	if (StructPro->Struct != DT->RowStruct)
	{
		return false;
	}
	TMap<FName, uint8*>& DTMap = const_cast<TMap<FName, uint8*>&>(DT->GetRowMap());
	UScriptStruct& EmptyUsingStruct = *DT->RowStruct;
	uint8* NewRawRowData = (uint8*)FMemory::Malloc(EmptyUsingStruct.GetStructureSize());
	EmptyUsingStruct.InitializeStruct(NewRawRowData);
	EmptyUsingStruct.CopyScriptStruct(NewRawRowData, Target);
	DTMap.Add(NewRowName, NewRawRowData);
	DT->Modify();
	FDataTableEditorUtils::BroadcastPostChange(DT, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	FDataTableEditorUtils::BroadcastPostChange(DT, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	return true;
}

void ULevelCreatorLibrary::GetObjThumbnail(int32 ImageRes, UObject* InObject, UTexture2D* InTexture2d)
{
	FObjectThumbnail ObjThumnail;
	//= ThumbnailTools::GetThumbnailForObject(InObject);
	ThumbnailTools::RenderThumbnail(InObject, ImageRes, ImageRes, ThumbnailTools::EThumbnailTextureFlushMode::AlwaysFlush, NULL, &ObjThumnail);
	TArray<uint8> ThumnailData = ObjThumnail.GetUncompressedImageData();

	TArray<FColor> ImageRawColor;

	for (int i =0; i < ThumnailData.Num(); i += 4)
	{
		ImageRawColor.Add(FColor(ThumnailData[i + 2], ThumnailData[i + 1], ThumnailData[i], ThumnailData[i + 3]));
	}

	if (ImageRawColor.Num() <= 0)
	{
		return;
	}

	uint8* Pixels = new uint8[ImageRes * ImageRes * 4];
	for (int32 y = 0; y < ImageRes; y++)
	{
		for (int32 x = 0; x < ImageRes; x++)
		{
			int32 curPixelIndex = ((y * ImageRes) + x);
			//这里可以设置4个通道的值
			//这里需要考虑像素格式，之前设置了PF_B8G8R8A8，那么这里通道的顺序就是BGRA
			Pixels[4 * curPixelIndex] = ImageRawColor[curPixelIndex].B;
			Pixels[4 * curPixelIndex + 1] = ImageRawColor[curPixelIndex].G;
			Pixels[4 * curPixelIndex + 2] = ImageRawColor[curPixelIndex].R;
			Pixels[4 * curPixelIndex + 3] = ImageRawColor[curPixelIndex].A;
		}
	}
	//创建第一个MipMap
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	InTexture2d->GetPlatformData()->Mips.Add(Mip);
	Mip->SizeX = ImageRes;
	Mip->SizeY = ImageRes;

	//锁定Texture让它可以被修改
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(ImageRes * ImageRes * 4);
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * ImageRes * ImageRes * 4);
	Mip->BulkData.Unlock();
	
	InTexture2d->Source.Init(ImageRes, ImageRes, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);
	InTexture2d->UpdateResource();
	
	InTexture2d->GetPackage()->MarkPackageDirty();
	
	return ;
}
