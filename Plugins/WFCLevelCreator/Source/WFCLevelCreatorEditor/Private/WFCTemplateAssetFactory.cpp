// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCTemplateAssetFactory.h"
#include "WFCTempalteAsset.h"

#define LOCTEXT_NAMESPACE "WFCTemplateAssetFactory"

UWFCTemplateAssetFactory::UWFCTemplateAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UWFCTemplateAsset::StaticClass();
}

UObject* UWFCTemplateAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
                                                    UObject* Context, FFeedbackContext* Warn)
{
	UWFCTemplateAsset* NewAsset = NewObject<UWFCTemplateAsset>(InParent, Class, Name, Flags | RF_Transactional);
	return NewAsset;
}

FText UWFCTemplateAssetFactory::GetToolTip() const
{
	return Super::GetToolTip();
}

bool UWFCTemplateAssetFactory::ConfigureProperties()
{
	return Super::ConfigureProperties();
}
#undef LOCTEXT_NAMESPACE
