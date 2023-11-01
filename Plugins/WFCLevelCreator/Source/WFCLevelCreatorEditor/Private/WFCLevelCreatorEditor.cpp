// Copyright Epic Games, Inc. All Rights Reserved.

#include "WFCLevelCreatorEditor.h"

#include "AssetToolsModule.h"
#include "EditorModeRegistry.h"
#include "FWFCUIStyle.h"
#include "IAssetTools.h"
#include "WFCEditorModeActorPicker.h"
#include "WFCRolesManagerAction.h"

#define LOCTEXT_NAMESPACE "FWFCLevelCreatorEditorModule"

EAssetTypeCategories::Type FWFCLevelCreatorEditorModule::WFCAssetCategory;

TSharedRef<FWFCRolesManagerToolkit> FWFCLevelCreatorEditorModule::CreateWFCRolesManagerAssetEditor(
	const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* WavesAsset)
{
	TSharedRef<FWFCRolesManagerToolkit> NewWFCRolesManagerAssetEditor(new FWFCRolesManagerToolkit());
	NewWFCRolesManagerAssetEditor->InitWFCRolesManagerEditor(Mode, InitToolkitHost, WavesAsset);
	return NewWFCRolesManagerAssetEditor;
}

void FWFCLevelCreatorEditorModule::StartupModule()
{
	FWFCUIStyle::Initialize();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	WFCAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("WFC")),LOCTEXT("WFCAssetCategory", "WFC"));

	auto RegisterAssetTypeAction = [&](TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		AssetActions.Add(Action);
	};
	
	RegisterAssetTypeAction(MakeShareable(new FWFCRolesManagerAction));

	FEditorModeRegistry::Get().RegisterMode<FWFCEdModeActorPicker>(
			FWFCEdModeActorPicker::EM_WFCManager,
			LOCTEXT("WFCEditMode", "WFC Editor Selector"),
			FSlateIcon(),
			false);
}

void FWFCLevelCreatorEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FAssetToolsModule* Module = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");
	
	FEditorModeRegistry::Get().UnregisterMode(FWFCEdModeActorPicker::EM_WFCManager);
	
	if (Module)
	{
		IAssetTools& AssetTools = Module->Get();
		for (auto Action : AssetActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
		AssetActions.Empty();
	}

	FWFCUIStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWFCLevelCreatorEditorModule, WFCLevelCreatorEditor)