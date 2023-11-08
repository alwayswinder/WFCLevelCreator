// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "UObject/Object.h"
#include "WFCTemplateAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class WFCLEVELCREATOREDITOR_API UWFCTemplateAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UWFCTemplateAssetFactory();
	
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetToolTip() const override;
	virtual bool ConfigureProperties() override;
};
