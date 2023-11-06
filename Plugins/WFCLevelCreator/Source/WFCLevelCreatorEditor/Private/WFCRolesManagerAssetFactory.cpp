// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManagerAssetFactory.h"

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "WFCRolesManager.h"
#include "WFCItemBase.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Widgets/SWidget.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "GameFramework/Character.h"

#define LOCTEXT_NAMESPACE "WFCRolesManagerAssetFactory"

class SWFCRolesManagerCreateDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SWFCRolesManagerCreateDialog ){}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs )
	{
		bOkClicked = false;
		ParentClass = AWFCItemBase::StaticClass();
		
		ChildSlot
		[
			SNew(SBorder)
			.Visibility(EVisibility::Visible)
			.BorderImage(FAppStyle::GetBrush("ChildWindow.Background"))
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(10.0f)
					[
						SNew(SBox)
						.Visibility(EVisibility::Visible)
						.WidthOverride(400.0f)
						.HeightOverride(400.0f)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("NewAnimBlueprintDialog.AreaBorder"))
							[
								SNew(SVerticalBox)
					 			+SVerticalBox::Slot()
								.FillHeight(1.0f)
								[
									MakeWFCItemPickerArea()
								]
							]
						]
					]
					+SVerticalBox::Slot()
					.AutoHeight()
					.Padding(10.0f, 0.0f, 10.0f, 0.0f)
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("NewAnimBlueprintDialog.AreaBorder"))
						[
							MakeParentClassPicker()
						]
					]
				]

				// Ok/Cancel buttons
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(10.0f)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
					.MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
					.MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
					+SUniformGridPanel::Slot(0,0)
					[
						SNew(SButton)
						.ToolTipText(LOCTEXT("CreateWFCRolesManagerCreate_Tooltip", "Create a new animation Blueprint.\nSelect a target skeleton or whether the animation Blueprint should be a template.\nOptionally select a parent class."))
						.IsEnabled_Lambda([this]()
						{
							return ParentClass.Get() != nullptr;
						})
						.HAlign(HAlign_Center)
						.ContentPadding( FAppStyle::GetMargin("StandardDialog.ContentPadding") )
						.OnClicked(this, &SWFCRolesManagerCreateDialog::OkClicked)
						.Text(LOCTEXT("CreateWFCRolesManagerCreate", "Create"))
					]
					+SUniformGridPanel::Slot(1,0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.ContentPadding( FAppStyle::GetMargin("StandardDialog.ContentPadding") )
						.OnClicked(this, &SWFCRolesManagerCreateDialog::CancelClicked)
						.Text(LOCTEXT("CreateWFCRolesManagerCancel", "Cancel"))
					]
				]
			]
		];
	}
	
	/** Sets properties for the supplied AnimBlueprintFactory */
	bool ConfigureProperties(TWeakObjectPtr<UWFCRolesManagerAssetFactory> InWFcRolesManagerFactory)
	{
		WFCRolesManagerFactory = InWFcRolesManagerFactory;

		TSharedRef<SWindow> Window = SNew(SWindow)
		.Title( LOCTEXT("CreateWFCRolesManagerOptions", "Create WFC Items List") )
		.SizingRule(ESizingRule::Autosized)
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			AsShared()
		];

		PickerWindow = Window;

		GEditor->EditorAddModalWindow(Window);
		WFCRolesManagerFactory.Reset();

		return bOkClicked;
	}

private:
	class FWFCItemParentFilter : public IClassViewerFilter
	{
	public:
		/** All children of these classes will be included unless filtered out by another setting. */
		TSet< const UClass* > AllowedChildrenOfClasses;

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs ) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet( AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet( AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
		}
	};

	/** Creates the combo menu for the parent class */
	TSharedRef<SWidget> MakeParentClassPicker()
	{
		// Load the classviewer module to display a class picker
		FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

		// Fill in options
		FClassViewerInitializationOptions Options;
		Options.Mode = EClassViewerMode::ClassPicker;
		Options.DisplayMode = EClassViewerDisplayMode::TreeView;
		Options.InitiallySelectedClass = AWFCItemBase::StaticClass();

		
		Options.bShowDefaultClasses = false;
		// Only allow parenting to base blueprints.
		Options.bIsBlueprintBaseOnly = true;

		TSharedPtr<FWFCItemParentFilter> Filter = MakeShared<FWFCItemParentFilter>();
		Options.ClassFilters.Add(Filter.ToSharedRef());

		// All child child classes of UAnimInstance are valid.
		Filter->AllowedChildrenOfClasses.Add(AWFCItemBase::StaticClass());

		return
			SNew(SExpandableArea)
			.ToolTipText(LOCTEXT("ParentClass_Tooltip", "Optionally choose a parent class for your Animation Blueprint"))
			.Padding(10.0f)
			.InitiallyCollapsed(true)
			.MaxHeight(200)
			.HeaderContent()
			[
				SNew(SBox)
				.Padding(5.0f)
				[
					SNew(STextBlock)
					.Text_Lambda([this]()
					{
						return FText::Format(LOCTEXT("ParentClassFormat", "Parent Class: {0}"), FText::FromString(ParentClass->GetName()));
					})
					.TextStyle( FAppStyle::Get(), "NormalText" )
				]
			]
			.BodyContent()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SWFCRolesManagerCreateDialog::OnClassPicked))
			];
	}

	/** Handler for when a parent class is selected */
	void OnClassPicked(UClass* ChosenClass)
	{
		if(ChosenClass != nullptr)
		{
			ParentClass = ChosenClass;
			RefreshWFCitemPicker();
		}
	}

	void RefreshWFCitemPicker()
	{
		RefreshWFCItemViewDelegate.ExecuteIfBound(true);
	}
	
	/** Creates the widgets for the target skeleton area */
	TSharedRef<SWidget> MakeWFCItemPickerArea()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

		FAssetPickerConfig AssetPickerConfig;
		AssetPickerConfig.RefreshAssetViewDelegates.Add(&RefreshWFCItemViewDelegate);
		AssetPickerConfig.Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
		AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SWFCRolesManagerCreateDialog::OnWFCItemSelected);
		AssetPickerConfig.GetCurrentSelectionDelegates.Add(&GetSelectedReferencerAssets);
		AssetPickerConfig.OnShouldFilterAsset = FOnShouldFilterAsset::CreateSP(this, &SWFCRolesManagerCreateDialog::FilterWFCItemBasedOnParentClass);
		AssetPickerConfig.bAllowNullSelection = true;
		AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
		//AssetPickerConfig.InitialAssetSelection = TargetSkeleton;
		AssetPickerConfig.HiddenColumnNames =
		{
			"DiskSize",
			"AdditionalPreviewSkeletalMeshes",
			"PreviewSkeletalMesh"
		};
		AssetPickerConfig.bShowPathInColumnView = false;
		AssetPickerConfig.bShowTypeInColumnView = false;
		AssetPickerConfig.bFocusSearchBoxWhenOpened = false;
	
		return SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(5.0f)
			[
				ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
			];
	}

	bool FilterWFCItemBasedOnParentClass(const FAssetData& AssetData)
	{
		UBlueprint *AssetType = Cast<UBlueprint>(AssetData.GetAsset());
		if(AssetType->ParentClass == ParentClass.Get() || AssetType->ParentClass->IsChildOf(ParentClass.Get()))
		{
			return false;
		}
		return true;
	}

	/** Handler for when a skeleton is selected */
	void OnWFCItemSelected(const FAssetData& AssetData)
	{
		TargetRes = GetSelectedReferencerAssets.Execute();
	}

	/** Handler for when ok is clicked */
	FReply OkClicked()
	{
		if(WFCRolesManagerFactory.IsValid())
		{
			for (auto AssetData : TargetRes)
			{
				UBlueprint *AssetType = Cast<UBlueprint>(AssetData.GetAsset());
				TSubclassOf<AWFCItemBase> WFCItemClass = static_cast<TSubclassOf<AWFCItemBase>>(AssetType->GeneratedClass);
				(WFCRolesManagerFactory->WFCItemsClasses).Add(WFCItemClass);
			}
		}
		CloseDialog(true);

		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked=false)
	{
		bOkClicked = bWasPicked;
		if ( PickerWindow.IsValid() )
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	/** Handler for when cancel is clicked */
	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

	FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			CloseDialog();
			return FReply::Handled();
		}
		return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

private:
	/** The factory for which we are setting up properties */
	TWeakObjectPtr<UWFCRolesManagerAssetFactory> WFCRolesManagerFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The container for the target skeleton picker*/
	TSharedPtr<SVerticalBox> WFCItemContainer;

	/** The selected class */
	TWeakObjectPtr<UClass> ParentClass;

	/** The selected skeleton */
	TArray<FAssetData> TargetRes;

	/** Delegate called to refresh the skeleton view */
	FRefreshAssetViewDelegate RefreshWFCItemViewDelegate;
	
	/** True if Ok was clicked */
	bool bOkClicked;

	FGetCurrentSelectionDelegate GetSelectedReferencerAssets;
};
UWFCRolesManagerAssetFactory::UWFCRolesManagerAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UWFCRolesManagerAsset::StaticClass();
}

UObject* UWFCRolesManagerAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UWFCRolesManagerAsset* NewAsset = NewObject<UWFCRolesManagerAsset>(InParent, Class, Name, Flags | RF_Transactional);
	NewAsset->InitItemClasses(WFCItemsClasses);
	return NewAsset;
}

FText UWFCRolesManagerAssetFactory::GetToolTip() const
{
	return LOCTEXT("WFCAssetTooltip", "Creates a default WFC Roles Manager asset");
}

bool UWFCRolesManagerAssetFactory::ConfigureProperties()
{
	TSharedRef<SWFCRolesManagerCreateDialog> Dialog = SNew(SWFCRolesManagerCreateDialog);
	return Dialog->ConfigureProperties(this);
}

#undef LOCTEXT_NAMESPACE
