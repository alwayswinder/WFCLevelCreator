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
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "WFC Grid Item")
	void OnHonvered(bool IsHovered);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "WFC Grid Item")
	void OnDragged(bool IsHovered);
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
	void UpdateGridSetting(UWFCRolesManagerAsset* RolesManager);
	void SetGridItemsHidenInEditor(bool IsHid);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_X = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_Y = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	float GridSize = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	TMap<FIntVector, AWFCGridItemBase*> GridItemsSave;
};
