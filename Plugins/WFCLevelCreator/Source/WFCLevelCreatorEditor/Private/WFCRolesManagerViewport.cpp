// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManagerViewport.h"

//#include <winsock2.h>

#include "WFCGridManager.h"
#include "WFCRolesManager.h"
#include "AdvancedPreviewScene.h"
#include "AssetEditorModeManager.h"
#include "EditorModeManager.h"
#include "EditorModes.h"
#include "Selection.h"
#include "UnrealWidget.h"
#include "WFCEditorModeActorPicker.h"
#include "WFCRolesManagerToolkit.h"


SWFCRolesManagerViewport::SWFCRolesManagerViewport()
{
	PreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));
}

void SWFCRolesManagerViewport::Construct(const FArguments& InArgs)
{
	WFCRolesManagerEditorToolkitPtr = InArgs._WFCRolesManagerEditorToolkit;
	check(WFCRolesManagerEditorToolkitPtr.IsValid());

	TSharedPtr<FWFCRolesManagerToolkit> WFCRolesManagerEditorToolkit = WFCRolesManagerEditorToolkitPtr.Pin();

	WFCRolesManagerAsset = WFCRolesManagerEditorToolkit->GetWFCRolesManagerAssetRef();
	
	SEditorViewport::Construct(SEditorViewport::FArguments());

	PreviewScene->SetFloorVisibility(false);

	/*生成网格actor*/
	WFCGridManager = CastChecked<AWFCGridManager>(PreviewScene->GetWorld()->SpawnActor(WFCRolesManagerAsset->WFCGridManagerClass));
	WFCRolesManagerAsset->WFCGridManagerRef = WFCGridManager;
	WFCGridManager->RolesManager = WFCRolesManagerAsset;
	WFCGridManager->UpdateGridSetting();
	float Offsetx = WFCRolesManagerAsset->Num_X * WFCRolesManagerAsset->GridSize * -0.7f;
	float Offsety = WFCRolesManagerAsset->Num_Y * WFCRolesManagerAsset->GridSize * 0.5;
	float Offsetz = WFCRolesManagerAsset->Num_Y * WFCRolesManagerAsset->GridSize * 1.2f  + 200.f;

	EditorViewportClient->MoveViewportCamera(FVector(Offsetx, Offsety, Offsetz), FRotator(-35.f, 0.f, 0.f));
	
}

TSharedRef<SEditorViewport> SWFCRolesManagerViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SWFCRolesManagerViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SWFCRolesManagerViewport::OnFloatingButtonClicked()
{
}

void SWFCRolesManagerViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(WFCRolesManagerAsset);

	Collector.AddReferencedObject(WFCGridManager);

}

TSharedRef<FEditorViewportClient> SWFCRolesManagerViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FWFCRolesManagerEditorViewportClient(PreviewScene.Get(),
		WFCRolesManagerAsset, SharedThis(this)));
	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SWFCRolesManagerViewport::MakeViewportToolbar()
{
	return SNew(SCommonEditorViewportToolbarBase, SharedThis(this));
}

EVisibility SWFCRolesManagerViewport::GetTransformToolbarVisibility() const
{
	return EVisibility::Visible;
}

FWFCRolesManagerEditorViewportClient::FWFCRolesManagerEditorViewportClient(
	FPreviewScene* InPreviewScene,
	UWFCRolesManagerAsset* InWFCAsset,
    const TWeakPtr<SEditorViewport>& InEditorViewportWidget)
	:FEditorViewportClient(nullptr, InPreviewScene, InEditorViewportWidget)
{
	WFCRolesManagerAsset = InWFCAsset;
	
	PreviewScene = InPreviewScene;
	bSetListenerPosition = false;
	SetRealtime(true);
	EngineShowFlags.Grid = false;
	
	//ModeTools->SetWidgetMode(UE::Widget::WM_Translate);
	Widget->SetUsesEditorModeTools(ModeTools.Get());
	
	((FAssetEditorModeManager*)ModeTools.Get())->SetPreviewScene(PreviewScene);
	
	 ModeTools->SetDefaultMode(FWFCEdModeActorPicker::EM_WFCManager);
	 ModeTools->ActivateDefaultMode();
	 FWFCEdModeActorPicker* Mode = ModeTools->GetActiveModeTyped<FWFCEdModeActorPicker>(FWFCEdModeActorPicker::EM_WFCManager);
	 if (ensure(Mode))
	 {
	 	Mode->OnActorFilled.BindRaw(this, &FWFCRolesManagerEditorViewportClient::OnActorFilled);
	 	Mode->OnActorHovered.BindRaw(this, &FWFCRolesManagerEditorViewportClient::OnActorHovered);
	 }
}

void FWFCRolesManagerEditorViewportClient::OnActorFilled(AActor* ActorSelected, bool IsAdd)
{
	if(!WFCRolesManagerAsset->bFillMode)
	{
		return;
	}
	ModeTools->GetSelectedObjects()->Select(ActorSelected);
	AWFCGridItemBase* SelectActor = Cast<AWFCGridItemBase>(ActorSelected);
	if(SelectActor)
	{
		SelectActor->OnFilled(IsAdd);
	}
}

void FWFCRolesManagerEditorViewportClient::OnActorHovered(AActor* ActorSelected)
{
	// AWFCGridItemBase* HoveredActor = Cast<AWFCGridItemBase>(ActorSelected);
	// if(HoveredActor)
	// {
	// 	HoveredActor->OnHonvered(true);
	// }
	// if(HoveredActor != nullptr && HoveredActor != LastHoveredActor)
	// {
	// 	HoveredActor->OnHonvered(true);
	// 	if(LastHoveredActor != nullptr)
	// 	{
	// 		LastHoveredActor->OnHonvered(false);
	// 	}
	// 	LastHoveredActor = HoveredActor;
	// }
}

void FWFCRolesManagerEditorViewportClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!GIntraFrameDebuggingGameThread)
	{
		PreviewScene->GetWorld()->Tick(IsRealtime() ? LEVELTICK_All : LEVELTICK_TimeOnly, DeltaSeconds);
	}
}

