// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManager.h"
#include "LevelCreatorLibrary.h"
#include "WFCItemBase.h"
#include "WFCTempalteAsset.h"
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

	if(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWFCRolesManagerAsset, StartIndex))
	{
		NextIndex = StartIndex;
		Modify();
	}
}

void UWFCRolesManagerAsset::SaveToTemplate()
{
	if( WFCTemplate)
	{
		WFCTemplate->DataReset(Num_X, Num_Y, GridSize, WFCItemClasses, SpawnedIndex, RotationsIndex);
	}
}

void UWFCRolesManagerAsset::LoadFromTemplate()
{
	if( WFCTemplate)
	{
		WFCGridManagerRef->ClearGridAll();
		Num_X = WFCTemplate->Num_X;
		Num_Y = WFCTemplate->Num_Y;
		GridSize = WFCTemplate->GridSize;
		WFCItemClasses = WFCTemplate->itemsClass;
		SpawnedIndex = WFCTemplate->SpawnedIndex;
		RotationsIndex = WFCTemplate->RotationsIndex;

		if(WFCGridManagerRef)
		{
			WFCGridManagerRef->InitGridAfterSpawn();
		}
		Modify();
	}
}

void UWFCRolesManagerAsset::ClearAll()
{
	if(WFCGridManagerRef)
	{
		WFCGridManagerRef->ClearGridAll();
	}
	
	AllTilesAdapt.Empty();
	ClassNumFilled.Empty();
	TargetFrequency.Empty();
	CurrentFrequence.Empty();
	
	//get total frequency
	float totalFrequency = 0;
	for (auto itemClass : WFCItemClasses)
	{
		AWFCItemBase* DefaultObj = Cast<AWFCItemBase>(itemClass->GetDefaultObject());
		totalFrequency += DefaultObj->Probability;
	}
	
	FWFCTilesAdapt tmpTilesAdapt;
	for (int c =0; c<WFCItemClasses.Num(); c++)
	{
		//fill all rot
		for (int r=0; r<4; r++)
		{
			tmpTilesAdapt.TilesAdapt.Add(FWFCTileInfo(c, r));
		}
		AWFCItemBase* DefaultObj = Cast<AWFCItemBase>(WFCItemClasses[c]->GetDefaultObject());
		//init Frequency
		ClassNumFilled.Add(0);
		TargetFrequency.Add(DefaultObj->Probability / totalFrequency);
		CurrentFrequence.Add(0);
	}
	
	for (int x=0; x<Num_X; x++)
	{
		for (int y=0; y<Num_Y; y++)
		{
			AllTilesAdapt.Add(FIntVector(x, y,0), tmpTilesAdapt);
		}
	}

	NextIndex = StartIndex;
}

void UWFCRolesManagerAsset::InitItemClasses(TArray<TSubclassOf<AWFCItemBase>> classes)
{
	WFCItemClasses = classes;
	InitThumbnails();
}

void UWFCRolesManagerAsset::InitThumbnails()
{
	Thumbnails.Empty();
	Brushes.Empty();
	int32 ImageRes = 64;
	
	for (auto itemClass : WFCItemClasses)
	{
		AWFCItemBase* DefaultObj = Cast<AWFCItemBase>(itemClass->GetDefaultObject());
		if(DefaultObj != nullptr && DefaultObj->ShowIcon != nullptr)
		{
			Thumbnails.Add(DefaultObj->ShowIcon);
			FSlateBrush TempBrushOutput;
			TempBrushOutput.SetResourceObject(DefaultObj->ShowIcon);
			TempBrushOutput.ImageSize = FVector2D(ImageRes, ImageRes);
			Brushes.Add(TempBrushOutput);
		}
		else
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
}

FSlateBrush* UWFCRolesManagerAsset::GetBrushByIndex(int32 index)
{
	if(Brushes.IsValidIndex(index))
	{
		return &Brushes[index];
	}
	return nullptr;
}

void UWFCRolesManagerAsset::GenerateNeighborInfo()
{
	NeighborLR.Empty();
	NeighborFB.Empty();
	
	FScopedSlowTask SlowTaskFB(static_cast<float>(WFCItemClasses.Num()), NSLOCTEXT("UnrealEd", "Generate", "Generate Neighbor Info..."));
    SlowTaskFB.MakeDialog();
    
	for (int c1 =0; c1<WFCItemClasses.Num(); c1++)
	{
		for (int r1=0; r1<4; r1++)
		{
			for (int c2 =0; c2<WFCItemClasses.Num(); c2++)
			{
				for (int r2=0; r2<4; r2++)
				{
					if(WFCGridManagerRef->IsAdaptLR(WFCItemClasses[c1], WFCItemClasses[c2], r1, r2))
					{
						NeighborLR.Add(FWFCNeighborInfo(c1, r1, c2, r2));
					}
					
					if(WFCGridManagerRef->IsAdaptFB(WFCItemClasses[c1], WFCItemClasses[c2], r1, r2))
					{
						NeighborFB.Add(FWFCNeighborInfo(c1, r1, c2, r2));
					}
				}
			}	
		}
		SlowTaskFB.EnterProgressFrame(1);
	}
	Modify();
}

void UWFCRolesManagerAsset::DebugNeighborLR()
{
	WFCGridManagerRef->ClearGridAll();
	
	if(NeighborLR.IsValidIndex(NeighborIndex))
	{
		FWFCNeighborInfo NeighborShow = NeighborLR[NeighborIndex];
		
		WFCGridManagerRef->SpawnItem(FIntVector(0,0,0), NeighborShow.class_1, NeighborShow.rot_1);
		
		WFCGridManagerRef->SpawnItem(FIntVector(0,1,0), NeighborShow.class_2, NeighborShow.rot_2);
	}
	NeighborIndex++;
	Modify();
}

void UWFCRolesManagerAsset::DebugNeighborFb()
{
	WFCGridManagerRef->ClearGridAll();
	
	if(NeighborFB.IsValidIndex(NeighborIndex))
	{
		FWFCNeighborInfo NeighborShow = NeighborFB[NeighborIndex];
		
		WFCGridManagerRef->SpawnItem(FIntVector(1,0,0), NeighborShow.class_1, NeighborShow.rot_1);
		
		WFCGridManagerRef->SpawnItem(FIntVector(0,0,0), NeighborShow.class_2, NeighborShow.rot_2);
	}
	NeighborIndex++;
	Modify();
}

void UWFCRolesManagerAsset::GenerateItem()
{
	if(AllTilesAdapt.Num() > 0 && AllTilesAdapt.Contains(NextIndex))
	{
		//first select one to fill
		FWFCTilesAdapt CurrentTiles = AllTilesAdapt[NextIndex];
		if(CurrentTiles.TilesAdapt.Num() > 0)
		{
			
			//get min Frequence 
			float MinFrequence = 100.f;
			for (auto tileAdapt : CurrentTiles.TilesAdapt)
			{
				float tmpFrequence = (ClassNumFilled[tileAdapt.classIndex] / (Num_X * Num_Y * 1.f)) / TargetFrequency[tileAdapt.classIndex];
				if(tmpFrequence < MinFrequence)
				{
					MinFrequence = tmpFrequence;
				}
			}

			//get all min frequence tiles
			TArray<FWFCTileInfo> TmpTilesAdapt;
			for (auto tileAdapt : CurrentTiles.TilesAdapt)
			{
				float tmpFrequence =(ClassNumFilled[tileAdapt.classIndex] / (Num_X * Num_Y * 1.f))  / TargetFrequency[tileAdapt.classIndex];
				if(tmpFrequence <= (MinFrequence + 0.00001f))
				{
					TmpTilesAdapt.Add(tileAdapt);
				}
			}

			//Error!
			if(TmpTilesAdapt.Num() <= 0)
			{
				return;
			}
			
			//random one
			int selectIndex = FMath::RandRange(0, TmpTilesAdapt.Num()-1);
			int32 classindex = TmpTilesAdapt[selectIndex].classIndex;
			int32 rot = TmpTilesAdapt[selectIndex].rot;
			WFCGridManagerRef->SpawnItem(NextIndex, classindex, rot);
			AllTilesAdapt.Remove(NextIndex);
			ClassNumFilled[classindex] = ClassNumFilled[classindex] + 1;
			CurrentFrequence[classindex] = ClassNumFilled[classindex] / (Num_X * Num_Y * 1.f);
			Modify();
			
			//Filter left
			FIntVector leftindex = NextIndex - FIntVector(0, 1, 0);
			if(AllTilesAdapt.Contains(leftindex))
			{
				FWFCTilesAdapt leftTiles = 	AllTilesAdapt[leftindex];
				for (int t = leftTiles.TilesAdapt.Num()-1; t>=0; t--)
				{
					int32 c = leftTiles.TilesAdapt[t].classIndex;
					int32 r = leftTiles.TilesAdapt[t].rot;
					if(!WFCGridManagerRef->IsAdaptLR(WFCItemClasses[c], WFCItemClasses[classindex], r, rot))
					{
						leftTiles.TilesAdapt.RemoveAt(t);
					}
				}
				AllTilesAdapt[leftindex] = leftTiles;
			}
			//right
			FIntVector rightindex = NextIndex + FIntVector(0, 1, 0);
			if(AllTilesAdapt.Contains(rightindex))
			{
				FWFCTilesAdapt rightTiles = AllTilesAdapt[rightindex];
				for (int t = rightTiles.TilesAdapt.Num()-1; t>=0; t--)
				{
					int32 c = rightTiles.TilesAdapt[t].classIndex;
					int32 r = rightTiles.TilesAdapt[t].rot;
					if(!WFCGridManagerRef->IsAdaptLR(WFCItemClasses[classindex], WFCItemClasses[c], rot, r))
					{
						rightTiles.TilesAdapt.RemoveAt(t);
					}
				}
				AllTilesAdapt[rightindex] = rightTiles;
			}
			
			//Front
			FIntVector Frontindex = NextIndex + FIntVector(1, 0, 0);
			if(AllTilesAdapt.Contains(Frontindex))
			{
				FWFCTilesAdapt frontTiles = AllTilesAdapt[Frontindex];
				for (int t = frontTiles.TilesAdapt.Num()-1; t>=0; t--)
				{
					int32 c = frontTiles.TilesAdapt[t].classIndex;
					int32 r = frontTiles.TilesAdapt[t].rot;
					if(!WFCGridManagerRef->IsAdaptFB(WFCItemClasses[c], WFCItemClasses[classindex], r, rot))
					{
						frontTiles.TilesAdapt.RemoveAt(t);
					}
				}
				AllTilesAdapt[Frontindex] = frontTiles;
			}
			
			//back
			FIntVector backindex = NextIndex + FIntVector(-1, 0, 0);
			if(AllTilesAdapt.Contains(backindex))
			{
				FWFCTilesAdapt backTiles = AllTilesAdapt[backindex];
				for (int t = backTiles.TilesAdapt.Num()-1; t>=0; t--)
				{
					int32 c = backTiles.TilesAdapt[t].classIndex;
					int32 r = backTiles.TilesAdapt[t].rot;
					if(!WFCGridManagerRef->IsAdaptFB(WFCItemClasses[classindex], WFCItemClasses[c], rot, r))
					{
						backTiles.TilesAdapt.RemoveAt(t);
					}
				}
				AllTilesAdapt[backindex] = backTiles;
			}
			
			//find mixnum adapt one to do next
			int32 MinAdapt = WFCItemClasses.Num() * 4;
			for (auto TileAdapt : AllTilesAdapt)
			{
				FWFCTilesAdapt tmpadapt = TileAdapt.Value;
				if(tmpadapt.TilesAdapt.Num() < MinAdapt)
				{
					MinAdapt = tmpadapt.TilesAdapt.Num();
					NextIndex = TileAdapt.Key;
				}
			}
		}
		else//has none;  
		{
			//todo!
		}
	}
	else
	{
		//all has filled  or some one errer!
		//todo!
	}
}







