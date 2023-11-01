// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "WFCRolesManagerToolkit.h"
#include "Modules/ModuleManager.h"

class FWFCLevelCreatorEditorModule : public IModuleInterface
{
public:
	static EAssetTypeCategories::Type GetAssetCategory() { return WFCAssetCategory; }
	
	TSharedRef<FWFCRolesManagerToolkit> CreateWFCRolesManagerAssetEditor(const EToolkitMode::Type Mode,
		const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* WavesAsset);

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	TArray<TSharedPtr<class IAssetTypeActions>> AssetActions;
	
	static EAssetTypeCategories::Type WFCAssetCategory;
};
