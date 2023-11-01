﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManagerToolkit.h"

#include "ClassIconFinder.h"
#include "EditorViewportTabContent.h"
#include "LevelCreatorLibrary.h"
#include "WFCRolesManager.h"
#include "WFCRolesManagerEditorCommands.h"
#include "WFCRolesManagerViewport.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "WFCRolesmanagerEditorToolkit"

const FName FWFCRolesManagerToolkit::ViewportTabId(TEXT("WFCRolesManagerEditor_Viewport"));
const FName FWFCRolesManagerToolkit::PropertiesTabId(TEXT("WFCRolesManagerEditor_Properties"));

void SMyTileItem::Construct(const FArguments& InArgs)
{
	WFCAsset = InArgs._WFCAsset.Get();
	InputTileIndex = InArgs._InputTileIndex.Get();

	FSlateBrush* Brush;// = WFCAsset->GetBrushByIndex(InputTileIndex);
	ChildSlot
	[
		SAssignNew(Button, SButton)
		.OnHovered(this, &SMyTileItem::OnHovered)
		.OnUnhovered(this, &SMyTileItem::OnUnHovered)
		.OnPressed(this, &SMyTileItem::OnPressed)
		.ContentPadding(0)
		.Content()
		[
			SNew(SImage)
			.Image(Brush)
		]
	];
}

void SMyTileItem::OnHovered()
{
	Button->SetColorAndOpacity(FLinearColor(0.2, 0.2, 1, 1));
}

void SMyTileItem::OnUnHovered()
{
	if (!IsSelected)
	{
		Button->SetColorAndOpacity(FLinearColor::White);
	}
	else
	{
		Button->SetColorAndOpacity(FLinearColor::Green);
	}
}

void SMyTileItem::OnPressed()
{
	Button->SetColorAndOpacity(FLinearColor::Green);

	// if (WFCAsset->GetInputTileIndexSelected() != InputTileIndex)
	// {
	// 	WFCAsset->InputileIndexSelectedChange(InputTileIndex);
	// 	if (WFCAsset->LastSelected)
	// 	{
	// 		WFCAsset->LastSelected->OnNewBrushSelect();
	// 	}
	// 	WFCAsset->LastSelected = this;
	// }
	IsSelected = true;
}

void SMyTileItem::OnNewBrushSelect()
{
	Button->SetColorAndOpacity(FLinearColor::White);
	IsSelected = false;
}

void FWFCRolesManagerToolkit::InitWFCRolesManagerEditor(const EToolkitMode::Type Mode,
                                                        const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit)
{
	WfcRolesManagerAssetRef= CastChecked<UWFCRolesManagerAsset>(ObjectToEdit);
	
	FWFCRolesManagerEditorCommands::Register();
	BindCommands();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NotifyHook = this;

	WFCRolesManagerDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	WFCRolesManagerDetailsView->SetObject(WfcRolesManagerAssetRef);
	WFCRolesManagerDetailsView->HideFilterArea(true);
	
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_WFCRolesManagerEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
					->SetSizeCoefficient(0.9f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(ViewportTabId, ETabState::OpenedTab)
						->SetHideTabWell(true)
					)
					->Split
					(
						FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
						->SetSizeCoefficient(0.2f)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.7f)
							->AddTab(PropertiesTabId, ETabState::OpenedTab)
						)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, FName(TEXT("WFCRolesManagerEditorApp")), StandaloneDefaultLayout, bCreateDefaultToolbar, bCreateDefaultStandaloneMenu, ObjectToEdit);
	ExtendToolbar();
	RegenerateMenusAndToolbars();
	
	//WfcRolesManagerAssetRef->WFCGridManagerRef->GetWorld()->GetTimerManager().SetTimer(InitThumTimer, WfcRolesManagerAssetRef->WFCGridManagerRef.Get(),  &FWFCRolesManagerToolkit::InitThumbnails, 0.5f, false, 0.5f);
}

FName FWFCRolesManagerToolkit::GetToolkitFName() const
{
	return FName("WFCRolesManagerEditor");
}

FText FWFCRolesManagerToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "WFCRolesManager Editor");
}

FString FWFCRolesManagerToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "WFCRolesManager").ToString();
}

FLinearColor FWFCRolesManagerToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

void FWFCRolesManagerToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(WfcRolesManagerAssetRef);
}

void FWFCRolesManagerToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_WFCRolesManagerEditor", "WFC Roles Manager Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FWFCRolesManagerToolkit::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FWFCRolesManagerToolkit::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FWFCRolesManagerToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	
	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(PropertiesTabId);
}

bool FWFCRolesManagerToolkit::OnRequestClose()
{
	return FAssetEditorToolkit::OnRequestClose();
}

TSharedRef<SDockTab> FWFCRolesManagerToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	TSharedRef< SDockTab > DockableTab = SNew(SDockTab);

	ViewportTabContent = MakeShareable(new FEditorViewportTabContent());

	TWeakPtr<FWFCRolesManagerToolkit> WeakSharedThis = SharedThis(this);

	const FString LayoutId = FString("WFCRolesManagerEditorViewport");
	ViewportTabContent->Initialize([WeakSharedThis](const FAssetEditorViewportConstructionArgs& InConstructionArgs)
	{
		return SNew(SWFCRolesManagerViewport)
			.WFCRolesManagerEditorToolkit(WeakSharedThis);
	}, DockableTab, LayoutId);

	return DockableTab;
}

TSharedRef<SDockTab> FWFCRolesManagerToolkit::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);
	InputVbx = SNew(SVerticalBox);
	
	DetailsViewTab =  SNew(SDockTab)
		.Label(LOCTEXT("WFCRolesManagerEditorProperties_TabTitle", "Details"))
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				InputVbx.ToSharedRef()
			]
			+SScrollBox::Slot()
			[
				WFCRolesManagerDetailsView.ToSharedRef()
			]
		];
	return DetailsViewTab.ToSharedRef();
}

void FWFCRolesManagerToolkit::BindCommands()
{
	const FWFCRolesManagerEditorCommands& Commands = FWFCRolesManagerEditorCommands::Get();

	const TSharedRef<FUICommandList>& UICommandList = GetToolkitCommands();

	UICommandList->MapAction(Commands.ShowGrid,
		FExecuteAction::CreateSP(this, &FWFCRolesManagerToolkit::ToggleTest),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FWFCRolesManagerToolkit::IsPressed));
}

void FWFCRolesManagerToolkit::ExtendToolbar()
{
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FWFCRolesManagerToolkit::FillToolbar, GetToolkitCommands())
	);

	AddToolbarExtender(ToolbarExtender);
}

void FWFCRolesManagerToolkit::FillToolbar(FToolBarBuilder& ToolbarBuilder,
	const TSharedRef<FUICommandList> InToolkitCommands)
{
	const ISlateStyle* WFCUIStyle = FSlateStyleRegistry::FindSlateStyle(FWFCUIStyle::GetStyleSetName());

	ToolbarBuilder.BeginSection("WFC");
	{
		ToolbarBuilder.AddToolBarButton(FWFCRolesManagerEditorCommands::Get().ShowGrid);
	}
	ToolbarBuilder.EndSection();
}

void FWFCRolesManagerToolkit::ToggleTest()
{
	if(bPressed)
	{
		bPressed = false;
		WfcRolesManagerAssetRef->WFCGridManagerRef->SetGridItemsHidenInEditor(true);
		GEditor->RedrawAllViewports();
	}
	else
	{
		bPressed = true;
		WfcRolesManagerAssetRef->WFCGridManagerRef->SetGridItemsHidenInEditor(false);
		GEditor->RedrawAllViewports();
	}
}

bool FWFCRolesManagerToolkit::IsPressed() const
{
	return bPressed;
}
#undef LOCTEXT_NAMESPACE
