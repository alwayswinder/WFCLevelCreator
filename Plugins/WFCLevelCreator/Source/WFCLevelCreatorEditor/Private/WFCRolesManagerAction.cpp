// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManagerAction.h"

#include "WFCLevelCreatorEditor.h"
#include "WFCRolesManager.h"

uint32 FWFCRolesManagerAction::GetCategories()
{
	return FWFCLevelCreatorEditorModule::GetAssetCategory();
}

FText FWFCRolesManagerAction::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "FWFCRolesManagerAction", "WFCRolesManger");
}

UClass* FWFCRolesManagerAction::GetSupportedClass() const
{
	return UWFCRolesManagerAsset::StaticClass();
}

FColor FWFCRolesManagerAction::GetTypeColor() const
{
	return FColor::Blue;
}

void FWFCRolesManagerAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (auto Object = Cast<UWFCRolesManagerAsset>(*ObjIt))
		{
			FWFCLevelCreatorEditorModule* WFCEditorModule = &FModuleManager::LoadModuleChecked<FWFCLevelCreatorEditorModule>("WFCLevelCreatorEditor");
			WFCEditorModule->CreateWFCRolesManagerAssetEditor(Mode, EditWithinLevelEditor, Object);
		}
	}
}


