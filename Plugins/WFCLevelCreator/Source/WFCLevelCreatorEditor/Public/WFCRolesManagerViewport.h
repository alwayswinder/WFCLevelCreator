// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAssetEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class AWFCGridItemBase;
class AWFCItemBase;
class FWFCRolesManagerToolkit;
class FAdvancedPreviewScene;
class UWFCRolesManagerAsset;
class AWFCGridManager;
/**
 * 
 */
class WFCLEVELCREATOREDITOR_API SWFCRolesManagerViewport : public SAssetEditorViewport, public ICommonEditorViewportToolbarInfoProvider, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SWFCRolesManagerViewport) {}
		SLATE_ARGUMENT(TWeakPtr<FWFCRolesManagerToolkit>, WFCRolesManagerEditorToolkit)
	SLATE_END_ARGS()
	
	SWFCRolesManagerViewport();
	
	void Construct(const FArguments& InArgs);
	
	//~ ICommonEditorViewportToolbarInfoProvider interface
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	//~ FGCObject Interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("SWFCRolesManagerViewport");
	}
	
protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	virtual EVisibility GetTransformToolbarVisibility() const override;
private:
	/** The scene for this viewport. */
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;
	
	TSharedPtr<FEditorViewportClient> EditorViewportClient;

	TWeakPtr<FWFCRolesManagerToolkit> WFCRolesManagerEditorToolkitPtr;

	UWFCRolesManagerAsset* WFCRolesManagerAsset = nullptr;
		
	AWFCGridManager* WFCGridManager = nullptr;
};


class FWFCRolesManagerEditorViewportClient : public FEditorViewportClient
{
public:
	using Super = FEditorViewportClient;

	FWFCRolesManagerEditorViewportClient(FPreviewScene* InPreviewScene,
		UWFCRolesManagerAsset* InWFCAsset = nullptr,
		const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);

	//
	void OnActorSelected(AActor* ActorSelected);
	void OnActorRotate();

	//
	// FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	// End of FEditorViewportClient

	UWFCRolesManagerAsset* WFCRolesManagerAsset = nullptr;
	AWFCGridItemBase* LastSelectedActor = nullptr;

};