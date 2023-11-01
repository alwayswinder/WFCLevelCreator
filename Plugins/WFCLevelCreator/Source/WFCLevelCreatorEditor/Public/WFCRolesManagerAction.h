// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"

class WFCLEVELCREATOREDITOR_API FWFCRolesManagerAction : public FAssetTypeActions_Base
{
public:
 	virtual uint32 GetCategories()override;
 	virtual FText GetName()const override;
 	virtual UClass* GetSupportedClass()const override;
 	virtual FColor GetTypeColor()const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost>
	 EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
};
