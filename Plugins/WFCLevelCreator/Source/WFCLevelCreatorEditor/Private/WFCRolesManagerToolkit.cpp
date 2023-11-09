// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManagerToolkit.h"
#include "EditorViewportTabContent.h"
#include "WFCRolesManager.h"
#include "WFCRolesManagerEditorCommands.h"
#include "WFCRolesManagerViewport.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "WFCRolesmanagerEditorToolkit"

const FName FWFCRolesManagerToolkit::ViewportTabId(TEXT("WFCRolesManagerEditor_Viewport"));
const FName FWFCRolesManagerToolkit::PropertiesTabId(TEXT("WFCRolesManagerEditor_Properties"));

void SMyTileItem::Construct(const FArguments& InArgs)
{
	WFCAsset = InArgs._WFCAsset.Get();
	Toolkit = InArgs._Toolkit.Get();
	InputTileIndex = InArgs._InputTileIndex.Get();

	FSlateBrush* Brush = WFCAsset->GetBrushByIndex(InputTileIndex);
	ChildSlot
	[
		SAssignNew(Button, SButton)
		.OnPressed(this, &SMyTileItem::OnPressed)
		.ContentPadding(0)
		.Content()
		[
			SNew(SImage)
			.Image(Brush)
		]
	];
}

void SMyTileItem::OnPressed()
{
	WFCAsset->WFCGridManagerRef->SpawnItemByIndex(WFCAsset->SelectedGrid, InputTileIndex);
	WFCAsset->SelectedClassIndex = InputTileIndex;
	Toolkit->UpdateTilesSelectState();
}

void SMyTileItem::CheckSelected()
{
	if(InputTileIndex == WFCAsset->SelectedClassIndex)
	{
		Button->SetBorderBackgroundColor(FLinearColor::Green);
	}
	else
	{
		Button->SetBorderBackgroundColor(FLinearColor::White);
	}
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
	if(WfcRolesManagerAssetRef)
	{
		WfcRolesManagerAssetRef->WFCGridManagerRef->Destroy();
	}
	return true;
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
	InitThumbnails();

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

void FWFCRolesManagerToolkit::InitThumbnails()
{
	WfcRolesManagerAssetRef->InitThumbnails();
	FillInputItemsView();
}

TSharedRef<SVerticalBox> FWFCRolesManagerToolkit::FillInputItemsView()
{
	InputVbx->ClearChildren();
	
	InputVbx->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		[
			SNew(SButton)
			.Text(FText::FromString("Relod WFC Items!"))
			.OnPressed(this, &FWFCRolesManagerToolkit::InitThumbnails)
		];
	if (WfcRolesManagerAssetRef)
	{
		if(WfcRolesManagerAssetRef->Thumbnails.Num() > 0)
		{
			int32 RowEach = 5;
			int32 Rows = (WfcRolesManagerAssetRef->WFCItemClasses.Num() - 1) / RowEach;
			
			TileItems.Empty();
			
			for (int32 r=0; r<=Rows; r++)
			{
				TSharedPtr<SHorizontalBox> TmpHbx = SNew(SHorizontalBox);

				for (int32 c = 0; c < RowEach && r * RowEach + c < WfcRolesManagerAssetRef->WFCItemClasses.Num(); c++)
				{
					int32 TileIndex =  r * RowEach + c;
					TSharedPtr<SMyTileItem> TmpTileItem;
					
					TmpHbx->AddSlot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SBorder)
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							.Padding(0)
							[
								SAssignNew(TmpTileItem, SMyTileItem)
								.WFCAsset(WfcRolesManagerAssetRef)
								.Toolkit(this)
								.InputTileIndex(TileIndex)
							]
						];
					TileItems.Add(TmpTileItem);
				}
				InputVbx->AddSlot()
					.AutoHeight()
					.HAlign(HAlign_Left)
					[
						TmpHbx.ToSharedRef()
					];
			}
		}
	}
	return InputVbx.ToSharedRef();
}
void FWFCRolesManagerToolkit::BindCommands()
{
	const FWFCRolesManagerEditorCommands& Commands = FWFCRolesManagerEditorCommands::Get();

	const TSharedRef<FUICommandList>& UICommandList = GetToolkitCommands();

	UICommandList->MapAction(Commands.ShowGrid,
		FExecuteAction::CreateSP(this, &FWFCRolesManagerToolkit::ShowGrid),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FWFCRolesManagerToolkit::IsPressedShowGrid));
	
	UICommandList->MapAction(Commands.ShowDebug,
		FExecuteAction::CreateSP(this, &FWFCRolesManagerToolkit::ShowDebug),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FWFCRolesManagerToolkit::IsPressedShowDebug));
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
	ToolbarBuilder.BeginSection("WFC");
	{
		ToolbarBuilder.AddToolBarButton(FWFCRolesManagerEditorCommands::Get().ShowGrid);

		ToolbarBuilder.AddToolBarButton(FWFCRolesManagerEditorCommands::Get().ShowDebug);

	}
	ToolbarBuilder.EndSection();
}

void FWFCRolesManagerToolkit::UpdateTilesSelectState()
{
	for (auto TileItem : TileItems)
	{
		TileItem->CheckSelected();
	}
}

void FWFCRolesManagerToolkit::ShowGrid()
{
	if(WfcRolesManagerAssetRef->bShowGrid == true)
	{
		WfcRolesManagerAssetRef->WFCGridManagerRef->SetGridItemsHidenInEditor(true);
		WfcRolesManagerAssetRef->bShowGrid = false;
		WfcRolesManagerAssetRef->Modify();
		GEditor->RedrawAllViewports();
	}
	else
	{
		WfcRolesManagerAssetRef->WFCGridManagerRef->SetGridItemsHidenInEditor(false);
		WfcRolesManagerAssetRef->bShowGrid = true;
		WfcRolesManagerAssetRef->Modify();
		GEditor->RedrawAllViewports();
	}
}

void FWFCRolesManagerToolkit::ShowDebug()
{
	WfcRolesManagerAssetRef->bShowDebug = !WfcRolesManagerAssetRef->bShowDebug;
}


bool FWFCRolesManagerToolkit::IsPressedShowGrid() const
{
	return WfcRolesManagerAssetRef->bShowGrid;
}

bool FWFCRolesManagerToolkit::IsPressedShowDebug() const
{
	return WfcRolesManagerAssetRef->bShowDebug;
}
#undef LOCTEXT_NAMESPACE
