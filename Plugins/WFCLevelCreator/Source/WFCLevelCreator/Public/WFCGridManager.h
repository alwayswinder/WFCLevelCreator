// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCGridManager.generated.h"

class AWFCItemBase;
class UWFCTemplate;
class FAdvancedPreviewScene;
class UWFCRolesManagerAsset;
class UWFCGridSettings;
class AWFCGridItemBase;

UCLASS()
class WFCLEVELCREATOR_API AWFCGridItemBase : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "WFC Grid Item")
	FIntVector GridXY;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid Item")
	TObjectPtr<UWFCRolesManagerAsset> RolesManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid Item")
	TObjectPtr<AWFCGridManager> GridManager;
	
public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "WFC Grid Item")
	void OnFilled(bool IsAdd);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "WFC Grid Item")
	void OnGridSelected(bool IsSelected);
};

UCLASS()
class WFCLEVELCREATOR_API AWFCGridManager : public AActor
{
	GENERATED_BODY()

public:
	AWFCGridManager();
	// Sets default values for this actor's properties
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintImplementableEvent, CallInEditor, BlueprintCallable, Category = "WFC Grid")
	void ResetGrid();
	
	UFUNCTION(BlueprintImplementableEvent, CallInEditor, BlueprintCallable, Category = "WFC Grid")
	void SpawnItemByIndex(FIntVector Gridxy, int32 ClassIndex);
	
	UFUNCTION(BlueprintImplementableEvent, CallInEditor, BlueprintCallable, Category = "WFC Grid")
	void RemoveItem(FIntVector Gridxy);
	
	UFUNCTION(BlueprintImplementableEvent, CallInEditor, BlueprintCallable, Category = "WFC Grid")
	void RotateItem(FIntVector Gridxy);
	
	void InitGridAfterSpawn();
	
	void UpdateGridSetting();
	
	void SetGridItemsHidenInEditor(bool IsHid);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	TObjectPtr<UWFCRolesManagerAsset> RolesManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_X = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_Y = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	float GridSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	TMap<FIntVector, AWFCGridItemBase*> GridItemsSave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	TMap<FIntVector, AWFCItemBase*> WFCItemsSave;
};
