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
		SpawnedIndex.Empty();
		RotationsIndex.Empty();
	}

	if(GenerateType == EWFCGenerateType::Roles)
	{
		InitWithRoles();
	}
	else
	{
		InitWithPatterns();
	}
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

void UWFCRolesManagerAsset::Analyse()
{
	if(GenerateType == EWFCGenerateType::Roles)
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
		InitWithRoles();
	}
	else
	{
		AllPatterns.Empty();
		ClassNumFilled.Empty();
		CurrentFrequence.Empty();
		
		for (auto itemClass : WFCItemClasses)
		{
			ClassNumFilled.Add(0);
			CurrentFrequence.Add(0);
		}
		
		FScopedSlowTask SlowTaskFB(static_cast<float>(Num_X * Num_Y), NSLOCTEXT("UnrealEd", "Generate", "Generate Neighbor Info..."));
		SlowTaskFB.MakeDialog();
		
		for (int x=0; x<Num_X; x++)
		{
			for (int y=0; y<Num_Y; y++)
			{
				SlowTaskFB.EnterProgressFrame(1);

				ClassNumFilled[SpawnedIndex[FIntVector(x, y,0)]] += 1;
				
				//3*3 patterns to array
				FWFCPatternsInfo tmpPatterns;
				for (int dx=-1; dx<=1; dx++)
				{
					for (int dy=-1; dy<=1; dy++)
					{
						FWFCTileInfo tmpTile;
						FIntVector tmpIndex = FIntVector(x + dx, y + dy, 0);
						if (SpawnedIndex.Contains(tmpIndex))
						{
							tmpTile.classIndex = SpawnedIndex[tmpIndex];
							tmpTile.rot = RotationsIndex[tmpIndex];
						}
						else//out of grid
						{
							tmpTile.classIndex = -1;
							tmpTile.rot = 0;
						}
						tmpPatterns.PatternTiles.Add(tmpTile);
					}
				}

				//check if same patterns alreay added
				bool HasFind = false;
				for (auto Patterns : AllPatterns)
				{
					bool IsSame = true;
					for (int i=0; i<Patterns.PatternTiles.Num(); i++)
					{
						if(Patterns.PatternTiles[i].classIndex != tmpPatterns.PatternTiles[i].classIndex
							|| Patterns.PatternTiles[i].rot != tmpPatterns.PatternTiles[i].rot)
						{
							IsSame = false;
							break;
						}
					}
					if(IsSame)
					{
						HasFind = true;
						break;
					}
				}	
				if(!HasFind)
				{
					AllPatterns.Add(tmpPatterns);
				}
			}
		}

		TargetFrequency.Empty();
		int totalnum = Num_X * Num_Y;
		for (int i=0; i< ClassNumFilled.Num(); i++)
		{
			TargetFrequency.Add(ClassNumFilled[i] * 1.f / (totalnum * 1.f));
			ClassNumFilled[i] = 0;
		}
		InitWithPatterns();
	}
	Modify();
}

void UWFCRolesManagerAsset::AutoFillEmpty()
{
	for (int x=0; x<Num_X; x++)
	{
		for (int y=0; y<Num_Y; y++)
		{
			if(!SpawnedIndex.Contains(FIntVector(x, y, 0)))
			{
				WFCGridManagerRef->SpawnItem(FIntVector(x, y, 0), SelectedClassIndex, 0);
			}
		}
	}
}

void UWFCRolesManagerAsset::DebugShow()
{
	WFCGridManagerRef->ClearGridAll();

	switch (DebugType)
	{
	case EWFCDebugType::NeighborLR:
		if(NeighborLR.IsValidIndex(DebugIndex))
		{
			FWFCNeighborInfo NeighborShow = NeighborLR[DebugIndex];
		
			WFCGridManagerRef->SpawnItem(FIntVector(0,0,0), NeighborShow.class_1, NeighborShow.rot_1);
		
			WFCGridManagerRef->SpawnItem(FIntVector(0,1,0), NeighborShow.class_2, NeighborShow.rot_2);
		}
		break;
	case EWFCDebugType::NeighborFB:
		if(NeighborFB.IsValidIndex(DebugIndex))
		{
			FWFCNeighborInfo NeighborShow = NeighborFB[DebugIndex];
		
			WFCGridManagerRef->SpawnItem(FIntVector(1,0,0), NeighborShow.class_1, NeighborShow.rot_1);
		
			WFCGridManagerRef->SpawnItem(FIntVector(0,0,0), NeighborShow.class_2, NeighborShow.rot_2);
		}
		break;
	case EWFCDebugType::Patterns:
		if (AllPatterns.IsValidIndex(DebugIndex))
		{
			FWFCPatternsInfo PatternTiles = AllPatterns[DebugIndex];
			int index = 0;
			for (int x=0; x<3; x++)
			{
				for (int y=0; y<3; y++)
				{
					WFCGridManagerRef->SpawnItem(FIntVector(x,y,0), PatternTiles.PatternTiles[index].classIndex,
						 PatternTiles.PatternTiles[index].rot);
					index++;
				}
			}
		}
		break;
	}
	
	DebugIndex++;
	Modify();
}


void UWFCRolesManagerAsset::GenerateItem()
{
	if(GenerateType == EWFCGenerateType::Roles)
	{
		GenerateWithRoles();
	}
	else
	{
		GenerateWithPatterns();
	}
}

void UWFCRolesManagerAsset::GenerateWithRoles()
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
			
			//auto next?
			if(!GenerateStep)
			{
				GenerateWithRoles();
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

void UWFCRolesManagerAsset::GenerateWithPatterns()
{
	if (PatternsAdapts.Num() > 0 && PatternsAdapts.Contains(NextIndex) && PatternsAdapts[NextIndex].PatternAdapts.Num() > 0)
	{
		if(UseFrequence)
		{
			//select one from adapt
			float MinFrequence = 10.f;
			FWFCTileInfo SelectTile;
			TArray<int32> PatternsIndex = PatternsAdapts[NextIndex].PatternAdapts;
			for (int i=0; i<PatternsIndex.Num(); i++)
			{
				FWFCPatternsInfo AdaptPattern = AllPatterns[PatternsIndex[i]];
				FWFCTileInfo AdaptTile = AdaptPattern.PatternTiles[AdaptPattern.PatternTiles.Num() / 2];
				float tmpFrequence = (ClassNumFilled[AdaptTile.classIndex] / (Num_X * Num_Y * 1.f)) / TargetFrequency[AdaptTile.classIndex];

				if(tmpFrequence < MinFrequence)
				{
					MinFrequence = tmpFrequence;
					SelectTile = AdaptTile;
				}
			}
		
			WFCGridManagerRef->SpawnItem(NextIndex, SelectTile.classIndex, SelectTile.rot);
			ClassNumFilled[SelectTile.classIndex] = ClassNumFilled[SelectTile.classIndex] + 1;
			CurrentFrequence[SelectTile.classIndex] = ClassNumFilled[SelectTile.classIndex] / (Num_X * Num_Y * 1.f);
			PatternsAdapts.Remove(NextIndex);
		}
		else
		{
			TArray<int32> PatternsIndex = PatternsAdapts[NextIndex].PatternAdapts;
			int selectIndex = FMath::RandRange(0, PatternsIndex.Num()-1);
			FWFCPatternsInfo SelectedPattern = AllPatterns[PatternsIndex[selectIndex]];
			FWFCTileInfo FillTile = SelectedPattern.PatternTiles[SelectedPattern.PatternTiles.Num() / 2];
		
			WFCGridManagerRef->SpawnItem(NextIndex, FillTile.classIndex, FillTile.rot);
			PatternsAdapts.Remove(NextIndex);
		}
		//update neighbor
		for (int dx=-1; dx<=1; dx++)
		{
			for (int dy=-1; dy<=1; dy++)
			{
				FIntVector TmpIndex = FIntVector(NextIndex.X + dx, NextIndex.Y + dy, 0);
				// valid and  not filled
				if(IsValidIndex(TmpIndex) && !SpawnedIndex.Contains(TmpIndex))
				{
					FWFCPatternsInfo PatternTiles = GetPatternTiles(TmpIndex);
			
					FWFCPatternsAdapt tmpPatternsIndex = FilterAdaptPatternsIndex(TmpIndex,PatternTiles);
			
					PatternsAdapts.Add(TmpIndex, tmpPatternsIndex);
				}
			}
		}

		//Update nextindex
		int minAdaptNum = AllPatterns.Num() + 1;
		for (auto patterns : PatternsAdapts)
		{
			if(patterns.Value.PatternAdapts.Num() < minAdaptNum && patterns.Value.PatternAdapts.Num() > 0)
			{
				minAdaptNum = patterns.Value.PatternAdapts.Num();
				NextIndex = patterns.Key;
			}
		}

		if(!GenerateStep)
		{
			GenerateWithPatterns();
		}
	}
	else//end or error!
	{
		//todo!
		if(!GenerateStep)
		{
			FillOther();
			Replace();
		}
	}
	Modify();
}

void UWFCRolesManagerAsset::InitWithRoles()
{
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

void UWFCRolesManagerAsset::InitWithPatterns()
{
	PatternsAdapts.Empty();
	ClassNumFilled.Empty();
	CurrentFrequence.Empty();

	for (int c =0; c<WFCItemClasses.Num(); c++)
	{
		ClassNumFilled.Add(0);
		CurrentFrequence.Add(0);
	}
	
	for (int x=0; x<Num_X; x++)
	{
		for (int y=0; y<Num_Y; y++)
		{
			FWFCPatternsInfo PatternTiles = GetPatternTiles(FIntVector(x, y, 0));
			
			FWFCPatternsAdapt tmpPatternsIndex = GatherAdaptPatternsIndex(PatternTiles);
			
			PatternsAdapts.Add(FIntVector(x, y, 0), tmpPatternsIndex);
		}
	}

	int minAdaptNum = AllPatterns.Num() + 1;
	for (auto patterns : PatternsAdapts)
	{
		if(patterns.Value.PatternAdapts.Num() < minAdaptNum)
		{
			minAdaptNum = patterns.Value.PatternAdapts.Num();
			NextIndex = patterns.Key;
		}
	}
}

bool UWFCRolesManagerAsset::IsValidIndex(FIntVector inIndex)
{
	if(inIndex.X < 0 || inIndex.X >= Num_X || inIndex.Y < 0 || inIndex.Y >= Num_Y)
	{
		return false;
	}
	return  true;
}

FWFCPatternsInfo UWFCRolesManagerAsset::GetPatternTiles(FIntVector Index)
{
	FWFCPatternsInfo PatternTiles;
	//get patterns adapt
	for (int dx=-1; dx<=1; dx++)
	{
		for (int dy=-1; dy<=1; dy++)
		{
			FIntVector TmpIndex = FIntVector(Index.X + dx, Index.Y + dy, 0);
			if(IsValidIndex(TmpIndex))
			{
				if(SpawnedIndex.Contains(TmpIndex))
				{
					PatternTiles.PatternTiles.Add(FWFCTileInfo(SpawnedIndex[TmpIndex], RotationsIndex[TmpIndex]));
				}
				else
				{
					//-2 to mark can fill any tiles
					PatternTiles.PatternTiles.Add(FWFCTileInfo(-2, 0));
				}
			}
			else
			{
				//out of grid
				PatternTiles.PatternTiles.Add(FWFCTileInfo(-1, 0));
			}
		}
	}
	return  PatternTiles;
}

FWFCPatternsAdapt UWFCRolesManagerAsset::GatherAdaptPatternsIndex(FWFCPatternsInfo PatternTiles)
{
	FWFCPatternsAdapt tmpPatternsIndex;
	for (int i=0; i<AllPatterns.Num(); i++)
	{
		bool IsSame = true;
		for (int j=0; j<PatternTiles.PatternTiles.Num(); j++)
		{
			if(PatternTiles.PatternTiles[j].classIndex == -2)
			{
				if(AllPatterns[i].PatternTiles[j].classIndex == -1)
				{
					IsSame = false;
					break;
				}
			}
			else
			{
				if(AllPatterns[i].PatternTiles[j].classIndex != PatternTiles.PatternTiles[j].classIndex
					|| AllPatterns[i].PatternTiles[j].rot != PatternTiles.PatternTiles[j].rot)
				{
					IsSame = false;
					break;
				}
			}
		}
		if(IsSame)
		{
			tmpPatternsIndex.PatternAdapts.Add(i);
		}
	}
	
	return tmpPatternsIndex;
}

FWFCPatternsAdapt UWFCRolesManagerAsset::FilterAdaptPatternsIndex(FIntVector index, FWFCPatternsInfo PatternTiles)
{
	FWFCPatternsAdapt tmpPatternsIndex;
	TArray<int32> AdaptIndexs =  PatternsAdapts[index].PatternAdapts;
	for (int i=0; i<AdaptIndexs.Num(); i++)
	{
		int PatternIndex = AdaptIndexs[i];

		bool IsSame = true;
		for (int j=0; j<PatternTiles.PatternTiles.Num(); j++)
		{
			if(PatternTiles.PatternTiles[j].classIndex == -2)
			{
				if(AllPatterns[PatternIndex].PatternTiles[j].classIndex == -1)
				{
					IsSame = false;
					break;
				}
			}
			else
			{
				if(AllPatterns[PatternIndex].PatternTiles[j].classIndex != PatternTiles.PatternTiles[j].classIndex
					|| AllPatterns[PatternIndex].PatternTiles[j].rot != PatternTiles.PatternTiles[j].rot)
				{
					IsSame = false;
					break;
				}
			}
		}
		if(IsSame)
		{
			tmpPatternsIndex.PatternAdapts.Add(PatternIndex);
		}
	}
	return tmpPatternsIndex;
}

void UWFCRolesManagerAsset::ReAdaptByGridIndex(FIntVector GridIndex)
{
	FWFCTilesAdapt tmpAllTilesAdapt;
	
	for (int c =0; c<WFCItemClasses.Num(); c++)
	{
		//fill all rot
		for (int r=0; r<4; r++)
		{
			tmpAllTilesAdapt.TilesAdapt.Add(FWFCTileInfo(c, r));
		}
	}

	//Filter left
	FIntVector leftindex = GridIndex + FIntVector(0, -1, 0);
	//valid and spawn
	if(IsValidIndex(leftindex) && SpawnedIndex.Contains(leftindex))
	{
		for (int t = tmpAllTilesAdapt.TilesAdapt.Num()-1; t>=0; t--)
		{
			int32 c = tmpAllTilesAdapt.TilesAdapt[t].classIndex;
			int32 r = tmpAllTilesAdapt.TilesAdapt[t].rot;
			if(!WFCGridManagerRef->IsAdaptLR(WFCItemClasses[SpawnedIndex[leftindex]], WFCItemClasses[c],
				RotationsIndex[leftindex], r))
			{
				tmpAllTilesAdapt.TilesAdapt.RemoveAt(t);
			}
		}
	}
	//right
	FIntVector rightindex = GridIndex + FIntVector(0, 1, 0);
	if(IsValidIndex(rightindex) && SpawnedIndex.Contains(rightindex))
	{
		for (int t = tmpAllTilesAdapt.TilesAdapt.Num()-1; t>=0; t--)
		{
			int32 c = tmpAllTilesAdapt.TilesAdapt[t].classIndex;
			int32 r = tmpAllTilesAdapt.TilesAdapt[t].rot;
			if(!WFCGridManagerRef->IsAdaptLR(WFCItemClasses[c], WFCItemClasses[SpawnedIndex[rightindex]],
				r, RotationsIndex[rightindex]))
			{
				tmpAllTilesAdapt.TilesAdapt.RemoveAt(t);
			}
		}
	}

	//Front
	FIntVector Frontindex = GridIndex + FIntVector(1, 0, 0);
	if(IsValidIndex(Frontindex) && SpawnedIndex.Contains(Frontindex))
	{
		for (int t = tmpAllTilesAdapt.TilesAdapt.Num()-1; t>=0; t--)
		{
			int32 c = tmpAllTilesAdapt.TilesAdapt[t].classIndex;
			int32 r = tmpAllTilesAdapt.TilesAdapt[t].rot;
			if(!WFCGridManagerRef->IsAdaptFB(WFCItemClasses[SpawnedIndex[Frontindex]], WFCItemClasses[c], 
				RotationsIndex[Frontindex], r))
			{
				tmpAllTilesAdapt.TilesAdapt.RemoveAt(t);
			}
		}
	}

	//back
	FIntVector backindex = GridIndex + FIntVector(-1, 0, 0);
	if(IsValidIndex(backindex) && SpawnedIndex.Contains(backindex))
	{
		for (int t = tmpAllTilesAdapt.TilesAdapt.Num()-1; t>=0; t--)
		{
			int32 c = tmpAllTilesAdapt.TilesAdapt[t].classIndex;
			int32 r = tmpAllTilesAdapt.TilesAdapt[t].rot;
			if(!WFCGridManagerRef->IsAdaptFB(WFCItemClasses[c], WFCItemClasses[SpawnedIndex[backindex]],
				r, RotationsIndex[backindex]))
			{
				tmpAllTilesAdapt.TilesAdapt.RemoveAt(t);
			}
		}
	}

	int selectindex = FMath::RandRange(0, tmpAllTilesAdapt.TilesAdapt.Num() -1);
	if(tmpAllTilesAdapt.TilesAdapt.IsValidIndex(selectindex))
	{
		FWFCTileInfo selectTile = tmpAllTilesAdapt.TilesAdapt[selectindex];
		WFCGridManagerRef->SpawnItem(GridIndex, selectTile.classIndex, selectTile.rot);
	}
}

void UWFCRolesManagerAsset::FillOther()
{
	for (int x=0; x<Num_X; x++)
	{
		for (int y=0; y<Num_Y; y++)
		{
			FIntVector CurIndex = FIntVector(x, y, 0);

			//not filled
			if(!SpawnedIndex.Contains(CurIndex))
			{
				ReAdaptByGridIndex(CurIndex);
			}
		}
	}
	Modify();
}

void UWFCRolesManagerAsset::Replace()
{
	bool IsLoop = false;
	for (auto replace : ReplaceMap)
	{
		int32 oldindex = replace.Key;
		//*****
		int select = FMath::RandRange(0, replace.Value.PatternAdapts.Num()-1);
		int32 newindex = replace.Value.PatternAdapts[select];
		for (auto spawned :SpawnedIndex )
		{
			if(spawned.Value == oldindex)
				//&&(spawned.Key.X != 0 && spawned.Key.Y != 0 && spawned.Key.X != (Num_X -1) && spawned.Key.Y != (Num_Y -1)))
			{
				IsLoop = true;
				FIntVector GridIndex = spawned.Key;
				int rotsave = RotationsIndex[spawned.Key];
				WFCGridManagerRef->RemoveItem(GridIndex);
				WFCGridManagerRef->SpawnItem(GridIndex, newindex, rotsave);

				//re adapt neighbor
				//left
				FIntVector leftGridIndex = GridIndex + FIntVector(0, -1, 0);
				if(IsValidIndex(leftGridIndex) &&
					!WFCGridManagerRef->IsAdaptLR(WFCItemClasses[SpawnedIndex[leftGridIndex]],
						WFCItemClasses[newindex],
					RotationsIndex[leftGridIndex],rotsave))
				{
					WFCGridManagerRef->RemoveItem(leftGridIndex);
					ReAdaptByGridIndex(leftGridIndex);
				}
				//right
				FIntVector rightGridIndex = GridIndex + FIntVector(0, 1, 0);
				if(IsValidIndex(rightGridIndex) &&
					!WFCGridManagerRef->IsAdaptLR(WFCItemClasses[newindex],
						WFCItemClasses[SpawnedIndex[rightGridIndex]], 
					rotsave, RotationsIndex[rightGridIndex]))
				{
					WFCGridManagerRef->RemoveItem(rightGridIndex);
					ReAdaptByGridIndex(rightGridIndex);
				}
				//front
				FIntVector frontGridIndex = GridIndex + FIntVector(1, 0, 0);
				if(IsValidIndex(frontGridIndex) &&
					!WFCGridManagerRef->IsAdaptFB(WFCItemClasses[SpawnedIndex[frontGridIndex]],
						WFCItemClasses[newindex],
					RotationsIndex[frontGridIndex],rotsave))
				{

					WFCGridManagerRef->RemoveItem(frontGridIndex);
					ReAdaptByGridIndex(frontGridIndex);
				}
				//back
				FIntVector backGridIndex = GridIndex + FIntVector(-1, 0, 0);
				if(IsValidIndex(backGridIndex) &&
					!WFCGridManagerRef->IsAdaptFB(WFCItemClasses[newindex],
						WFCItemClasses[SpawnedIndex[backGridIndex]], 
					rotsave,RotationsIndex[backGridIndex]))
				{
					WFCGridManagerRef->RemoveItem(backGridIndex);
					ReAdaptByGridIndex(backGridIndex);
				}
			}
		}
	}
	if(IsLoop)
	{
		Replace();
	}
	Modify();
}







