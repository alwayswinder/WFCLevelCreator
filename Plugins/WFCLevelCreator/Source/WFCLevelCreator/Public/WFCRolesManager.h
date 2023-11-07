// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WFCGridManager.h"
#include "WFCItemBase.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "WFCRolesManager.generated.h"

DECLARE_DELEGATE( FOnInitThumbnailsDelegate);

UCLASS(BlueprintType)
class UWFCTemplate : public UDataAsset
{
	GENERATED_BODY()
public:
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Template")
	TObjectPtr<UWFCTemplate> WFCTemplates;
	
	void InitItemClasses(TArray<TSubclassOf<AWFCItemBase>> classes);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC itme")
	TArray<TSubclassOf<AWFCItemBase>> WFCItemClasses;
public:
	UPROPERTY(EditAnywhere, Category = "WFC Grid", DisplayName = "WFCGridManager Source")
	TSubclassOf<AWFCGridManager> WFCGridManagerClass = nullptr;
	
	TObjectPtr<AWFCGridManager> WFCGridManagerRef = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_X = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	int32 Num_Y = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Grid")
	float GridSize = 100.f;
	
public:
	FOnInitThumbnailsDelegate OnInitThumbnailsDelegate;
	void InitThumbnails();
	FSlateBrush* GetBrushByIndex(int32 index);
	TArray<UTexture2D*> Thumbnails;
	TArray<FSlateBrush> Brushes;
	int32 SelectedClassIndex = 0;
	
public:
	bool bFillMode = false;
};