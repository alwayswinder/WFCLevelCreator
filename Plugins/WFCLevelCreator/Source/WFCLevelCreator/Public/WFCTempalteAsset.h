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
	void DataReset(float InItemsize, FIntVector InManagerSize, TArray<TSubclassOf<AWFCItemBase>> InitemsClass,
		TArray<int> InItemsIndex, TArray<FVector> InItemsRotOffset)
	{
		ItemSize = InItemsize;
		ManagerSize = InManagerSize;
		itemsClass = InitemsClass;
		ItemsIndex = InItemsIndex;
		ItemsRotOffset = InItemsRotOffset;
		Modify();
	}
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WFC Template")
	float ItemSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WFC Template")
	FIntVector ManagerSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WFC Template")
	TArray<TSubclassOf<AWFCItemBase>> itemsClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WFC Template")
	TArray<int> ItemsIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WFC Template")
	TArray<FVector> ItemsRotOffset;
};