// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManager.h"
#include "LevelCreatorLibrary.h"
#include "WFCItemBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "UObject/ConstructorHelpers.h"

UWFCRolesManagerAsset::UWFCRolesManagerAsset()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint>FindObject(TEXT("/Script/Engine.Blueprint'/WFCLevelCreator/WFC/Core/BP_WFCGridManager.BP_WFCGridManager'"));
	if(FindObject.Object)
	{
		WFCGridManagerClass=(UClass*)FindObject.Object->GeneratedClass;
	}
}

void UWFCRolesManagerAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCRolesManagerAsset, Num_X) ||
	PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCRolesManagerAsset, Num_Y) ||
	PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCRolesManagerAsset, GridSize))
	{
		if(WFCGridManagerRef)
		{
			WFCGridManagerRef->UpdateGridSetting();
		}
	}
}

void UWFCRolesManagerAsset::InitItemClasses(TArray<TSubclassOf<AWFCItemBase>> classes)
{
	WFCItemClasses = classes;
	OnInitThumbnailsDelegate.ExecuteIfBound();
}

void UWFCRolesManagerAsset::InitThumbnails()
{
	Thumbnails.Empty();
	Brushes.Empty();
	int32 ImageRes = 64;

	for (auto itemClass : WFCItemClasses)
	{
		UTexture2D* ImageTmp = UTexture2D::CreateTransient(ImageRes, ImageRes);
		ULevelCreatorLibrary::GetObjThumbnail(ImageRes*4, itemClass->ClassGeneratedBy, ImageTmp);
		Thumbnails.Add(ImageTmp);

		FSlateBrush TempBrushOutput;
		TempBrushOutput.SetResourceObject(ImageTmp);
		TempBrushOutput.ImageSize = FVector2D(ImageRes, ImageRes);
		Brushes.Add(TempBrushOutput);
	}
}

FSlateBrush* UWFCRolesManagerAsset::GetBrushByIndex(int32 index)
{
	if(Brushes.IsValidIndex(index))
	{
		return &Brushes[index];
	}
	return nullptr;
}







