// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WFCGridManager.h"
#include "WFCItemBase.h"
#include "UObject/Object.h"
#include "WFCRolesManager.generated.h"

class UWFCTemplateAsset;


UENUM(BlueprintType)
enum class EWFCGenerateType : uint8
{
	Roles = 0,// from link type
	Patterns = 1,// from template
};

UENUM(BlueprintType)
enum class EWFCDebugType : uint8
{
	NeighborLR = 0, 
	NeighborFB = 1,
	Patterns = 2,
};


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
struct FWFCPatternsInfo
{
	GENERATED_BODY()
public:
	FWFCPatternsInfo(){};
	FWFCPatternsInfo(TArray<FWFCTileInfo> InArray)
	{
		PatternTiles = InArray;
	}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Tile")
	TArray<FWFCTileInfo> PatternTiles;
};

USTRUCT(BlueprintType)
struct FWFCPatternsAdapt
{
	GENERATED_BODY()
public:
	FWFCPatternsAdapt(){};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Tile")
	TArray<int32> PatternAdapts;
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
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Grid")
	TObjectPtr<AWFCGridManager> WFCGridManagerRef = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_X = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_Y = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WFC Grid")
	float GridSize = 100.f;
	

public:
	void InitThumbnails();
	FSlateBrush* GetBrushByIndex(int32 index);

public:
	//Editor
	TArray<TObjectPtr<UTexture2D>> Thumbnails;
	
	TArray<FSlateBrush> Brushes;
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Editor")
	int32 SelectedClassIndex = 0;
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Editor")
	FIntVector SelectedGrid;
	
	UPROPERTY(BlueprintReadWrite, Category = "WFC Editor")
	bool bShowGrid = true;
		
	UPROPERTY(BlueprintReadWrite, Category = "WFC Editor")
	bool bShowDebug = false;
	
public:
	//Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Debug")
	int32 DebugIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Debug")
	EWFCDebugType DebugType = EWFCDebugType::NeighborLR;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Debug")
	void DebugShow();

public:
	//Generate
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	EWFCGenerateType GenerateType = EWFCGenerateType::Roles;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	bool GenerateStep = false;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Generate")
	void Analyse();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Generate")
	void AutoFillEmpty();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "WFC Generate")
	void GenerateItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	FIntVector StartIndex;

	UPROPERTY(BlueprintReadWrite, Category = "WFC Generate")
	TMap<FIntVector, int32> SpawnedIndex;
	
	//0-0 1-90 2-180 3-270
	UPROPERTY(BlueprintReadWrite, Category = "WFC Generate")
	TMap<FIntVector, int32> RotationsIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TArray<float> TargetFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate")
	TArray<float> CurrentFrequence;
	
	//Roles
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate | Roles")
	TMap<FIntVector, FWFCTilesAdapt> AllTilesAdapt;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate | Roles")
	TArray<FWFCNeighborInfo> NeighborLR;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate | Roles")
	TArray<FWFCNeighborInfo> NeighborFB;

	//patterns
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate | Patterns")
	TArray<FWFCPatternsInfo> AllPatterns;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WFC Generate | Patterns")
	TMap<FIntVector, FWFCPatternsAdapt> AllPatternsAdapt;
	
private:
	FIntVector NextIndex;
	TArray<int32> ClassNumFilled;

	void GenerateWithRoles();
	void GenerateWithPatterns();
	void InitWithRoles();
	void InitWithPatterns();
	
};