// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WFCGridManager.h"
#include "WFCItemBase.h"
#include "UObject/Object.h"
#include "WFCRolesManager.generated.h"

class UWFCTemplateAsset;

USTRUCT(BlueprintType)
struct FWFCTileInfo
{
	GENERATED_BODY()
public:
	FWFCTileInfo(){};
	FWFCTileInfo(int32 c, int32 r)
	{
		classIndex = c;
		rot = r;
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Tile")
	int32 classIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Tile")
	int32 rot = 0;
};

USTRUCT(BlueprintType)
struct FWFCTilesAdapt
{
	GENERATED_BODY()
public:
	FWFCTilesAdapt(){};
	FWFCTilesAdapt(TArray<FWFCTileInfo> InArray)
	{
		TilesAdapt = InArray;
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Tile")
	TArray<FWFCTileInfo> TilesAdapt;
};

USTRUCT(BlueprintType)
struct FWFCNeighborInfo
{
	GENERATED_BODY()
public:
	FWFCNeighborInfo(){};
	FWFCNeighborInfo(int32 c1, int32 r1, int32 c2, int32 r2)
	{
		class_1 = c1;
		rot_1 = r1;
		class_2 = c2;
		rot_2 = r2;
	};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Neighbor")
	int32 class_1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Neighbor")
	int32 rot_1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Neighbor")
	int32 class_2 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Neighbor")
	int32 rot_2 = 0;
	
};

UCLASS(BlueprintType, Blueprintable, AutoExpandCategories = "WFCAssets")
class WFCLEVELCREATOR_API UWFCRolesManagerAsset : public UObject
{
	GENERATED_BODY()
public:
	UWFCRolesManagerAsset();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:
	//模板相关
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Template")
	TObjectPtr<UWFCTemplateAsset> WFCTemplate;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Template")
	void SaveToTemplate();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Template")
	void LoadFromTemplate();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Template")
	void ClearAll();
	
public:
	void InitItemClasses(TArray<TSubclassOf<AWFCItemBase>> classes);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC itme")
	TArray<TSubclassOf<AWFCItemBase>> WFCItemClasses;
	
public:
	//核心参数
	UPROPERTY(EditAnywhere, Category = "WFC Grid", DisplayName = "WFCGridManager Source")
	TSubclassOf<AWFCGridManager> WFCGridManagerClass = nullptr;
	
	TObjectPtr<AWFCGridManager> WFCGridManagerRef = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_X = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_Y = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WFC Grid")
	float GridSize = 100.f;
	
#if WITH_EDITOR
public:
	void InitThumbnails();
	FSlateBrush* GetBrushByIndex(int32 index);
#endif
	
#if WITH_EDITORONLY_DATA
public:	
	//UPROPERTY( BlueprintReadWrite, Category = "WFC Grid")
	TArray<TObjectPtr<UTexture2D>> Thumbnails;
	
	//UPROPERTY( BlueprintReadWrite, Category = "WFC Grid")
	TArray<FSlateBrush> Brushes;
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Grid")
	int32 SelectedClassIndex = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Grid")
	FIntVector SelectedGrid;
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Debug")
	bool bShowGrid = true;
		
	UPROPERTY(BlueprintReadWrite, Category = "WFC Debug")
	bool bShowDebug = false;
	
#endif
	UPROPERTY(BlueprintReadWrite, Category = "WFC Debug")
	TMap<FIntVector, int32> SpawnedIndex;

	//0-0 1-90 2-180 3-270
	UPROPERTY(BlueprintReadWrite, Category = "WFC Debug")
	TMap<FIntVector, int32> RotationsIndex;

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Generate")
	void GenerateNeighborInfo();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Debug")
	int32 NeighborIndex = 0;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Debug")
	void DebugNeighborLR();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Debug")
	void DebugNeighborFb();
	//neighbor info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TArray<FWFCNeighborInfo> NeighborLR;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TArray<FWFCNeighborInfo> NeighborFB;
	
public:
	//generate
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Generate")
	void GenerateItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TMap<FIntVector, FWFCTilesAdapt> AllTilesAdapt;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	FIntVector StartIndex;
private:
	FIntVector NextIndex;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TArray<float> TargetFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TArray<float> CurrentFrequence;
	
private:
	TArray<int32> ClassNumFilled;
};