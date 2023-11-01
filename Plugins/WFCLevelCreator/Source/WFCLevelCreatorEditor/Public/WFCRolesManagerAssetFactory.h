// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WFCRolesManagerAssetFactory.generated.h"

class AWFCItemBase;
/**
 * 
 */
UCLASS()
class WFCLEVELCREATOREDITOR_API UWFCRolesManagerAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UWFCRolesManagerAssetFactory();
	
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetToolTip() const override;
	virtual bool ConfigureProperties() override;
	// End of UFactory interface

	TArray<TSubclassOf<AWFCItemBase>> WFCItemsClasses;
};
