// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WFCTempalteAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, AutoExpandCategories = "WFCAssets")
class WFCLEVELCREATOR_API UWFCTemplateAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UWFCTemplateAsset();
	
	UFUNCTION(BlueprintCallable, Category = "WFC Tempalte")
	void DataReset(int32 X, int32 Y, float Size, TArray<TSubclassOf<AWFCItemBase>> Classes,
		TMap<FIntVector, int32> classIndex, TMap<FIntVector, int32> RotIndex)
	{
		Num_X = X;
		Num_Y = Y;
		GridSize = Size;
		itemsClass = Classes;
		SpawnedIndex = classIndex;
		RotationsIndex = RotIndex;
		Modify();
	}
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Template")
	int32 Num_X = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Template")
	int32 Num_Y = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WFC Template")
	float GridSize = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WFC Template")
	TArray<TSubclassOf<AWFCItemBase>> itemsClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WFC Template")
	TMap<FIntVector, int32> SpawnedIndex;

	//0-0 1-90 2-180 3-270
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WFC Template")
	TMap<FIntVector, int32> RotationsIndex;
	
};