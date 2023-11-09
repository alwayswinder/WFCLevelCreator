// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WFCRolesManager.h"

class FEditorViewportTabContent;
class FWFCRolesManagerToolkit;
/**
 * 
 */
class SMyTileItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyTileItem) {}
	SLATE_ATTRIBUTE(UWFCRolesManagerAsset*, WFCAsset)
	SLATE_ATTRIBUTE(FWFCRolesManagerToolkit*, Toolkit)
	SLATE_ATTRIBUTE(int32, InputTileIndex)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	void OnPressed();
	
public:
	void CheckSelected();
private:
	TSharedPtr<class SButton> Button;
	UWFCRolesManagerAsset* WFCAsset;
	FWFCRolesManagerToolkit* Toolkit;
	int32 InputTileIndex;
	bool IsSelected;
	
};
class FWFCRolesManagerToolkit : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
 
public:
    void InitWFCRolesManagerEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit);
	
	UWFCRolesManagerAsset* GetWFCRolesManagerAssetRef() { return WfcRolesManagerAssetRef; }

	//~ IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	//~ FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("FWFCRolesManagerEditorToolkit");
	}
	
private:
	static const FName ViewportTabId;
	static const FName PropertiesTabId;
	
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual bool OnRequestClose() override;

	/* Tab spawners */
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);
	TSharedRef<SVerticalBox> FillInputItemsView();
	
	void InitThumbnails();
	
	void BindCommands();
	void ExtendToolbar();
	void FillToolbar(FToolBarBuilder& ToolbarBuilder, const TSharedRef<FUICommandList> InToolkitCommands);
	
	
	void ShowGrid();
	void ShowDebug();
	bool IsPressedShowGrid() const;
	bool IsPressedShowDebug() const;
	
	
	TSharedPtr<FEditorViewportTabContent> ViewportTabContent;
	
	TSharedPtr<IDetailsView> WFCRolesManagerDetailsView;
	TSharedPtr<SDockTab> DetailsViewTab;
	TSharedPtr<SVerticalBox> InputVbx;
	TArray<TSharedPtr<SMyTileItem>> TileItems;
	
	UWFCRolesManagerAsset* WfcRolesManagerAssetRef = nullptr;

	FTimerHandle InitThumTimer;
public:
	void UpdateTilesSelectState();
};
