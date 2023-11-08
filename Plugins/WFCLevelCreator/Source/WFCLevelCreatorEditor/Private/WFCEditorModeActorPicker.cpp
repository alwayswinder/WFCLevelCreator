// Copyright Epic Games, Inc. All Rights Reserved.

#include "WFCEditorModeActorPicker.h"

#include "EditorModeManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SToolTip.h"
#include "EngineUtils.h"
#include "LevelEditorViewport.h"
#include "EditorModes.h"
#include "Widgets/SWindow.h"

#define LOCTEXT_NAMESPACE "PropertyPicker"

const FEditorModeID FWFCEdModeActorPicker::EM_WFCManager(TEXT("EM_WFCManager"));

FWFCEdModeActorPicker::FWFCEdModeActorPicker()
{
}

void FWFCEdModeActorPicker::Enter()
{
	FEdMode::Enter();
	PickState = EPickState::NotOverViewport;
	HoveredActor.Reset();
	// CursorDecoratorWindow = SWindow::MakeCursorDecorator();
	// FSlateApplication::Get().AddWindow(CursorDecoratorWindow.ToSharedRef(), true);
	// CursorDecoratorWindow->SetContent(
	// 	SNew(SToolTip)
	// 	.Text(this, &FWFCEdModeActorPicker::GetCursorDecoratorText)
	// 	);
}

void FWFCEdModeActorPicker::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	if(CursorDecoratorWindow.IsValid())
	{
		CursorDecoratorWindow->MoveWindowTo(FSlateApplication::Get().GetCursorPos() + FSlateApplication::Get().GetCursorSize());
	}

	FEdMode::Tick(ViewportClient, DeltaTime);
}

bool FWFCEdModeActorPicker::MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport,int32 x, int32 y)
{
	PickState = EPickState::OverViewport;
	HoveredActor.Reset();
	return FEdMode::MouseEnter(ViewportClient, Viewport, x, y);
}

bool FWFCEdModeActorPicker::MouseLeave(FEditorViewportClient* ViewportClient, FViewport* Viewport)
{
	//UE_LOG(LogTemp, Warning, TEXT("Mouse Leave!"));

	bMouseButtonDown = false;
	bShiftButtonDown = false;
	PickState = EPickState::NotOverViewport;
	HoveredActor.Reset();
	return FEdMode::MouseLeave(ViewportClient, Viewport);
}

bool FWFCEdModeActorPicker::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{
	//UE_LOG(LogTemp, Warning, TEXT("Mouse Move!"));
	//if (ViewportClient == GCurrentLevelEditingViewportClient)
	// if (ViewportClient)
	// {
	// 	PickState = EPickState::OverViewport;
	// 	HoveredActor.Reset();
	//
	// 	int32 HitX = Viewport->GetMouseX();
	// 	int32 HitY = Viewport->GetMouseY();
	// 	HHitProxy* HitProxy = Viewport->GetHitProxy(HitX, HitY);
	// 	if (HitProxy != NULL && HitProxy->IsA(HActor::StaticGetType()))
	// 	{
	// 		HActor* ActorHit = static_cast<HActor*>(HitProxy);
	// 		if(ActorHit->Actor != NULL)
	// 		{
	// 			AActor* Actor = ActorHit->Actor;
	// 			while (Actor->IsChildActor())
	// 			{
	// 				Actor = Actor->GetParentActor();
	// 			}
	// 			if(IsActorValid(Actor))
	// 			{
	// 				//OnActorHovered.ExecuteIfBound(Actor);
	// 			}
	// 			HoveredActor = Actor;
	// 			PickState =  IsActorValid(Actor) ? EPickState::OverActor : EPickState::OverIncompatibleActor;
	// 		}
	// 	}
	// }
	// else
	// {
	// 	PickState = EPickState::NotOverViewport;
	// 	HoveredActor.Reset();
	// }

	return true;
}

bool FWFCEdModeActorPicker::CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport,
	int32 InMouseX, int32 InMouseY)
{
	if(bMouseButtonDown || bShiftButtonDown)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Captured Mouse Move!"));
		int32 HitX = InViewport->GetMouseX();
		int32 HitY = InViewport->GetMouseY();
		HHitProxy* HitProxy = InViewport->GetHitProxy(HitX, HitY);
		if (HitProxy != NULL && HitProxy->IsA(HActor::StaticGetType()))
		{
			HActor* ActorHit = static_cast<HActor*>(HitProxy);
			if(ActorHit->Actor != NULL)
			{
				AActor* Actor = ActorHit->Actor;
				while (Actor->IsChildActor())
				{
					Actor = Actor->GetParentActor();
				}
				if(IsActorValid(Actor))
				{
					if (bMouseButtonDown && !bShiftButtonDown)
					{
						OnActorFilled.ExecuteIfBound(Actor, true);
					}
					else if(bMouseButtonDown && bShiftButtonDown)
					{
						OnActorFilled.ExecuteIfBound(Actor, false);
					}
					PickState = EPickState::OverActor;
				}
			}
		}
		else
		{
			PickState = EPickState::NotOverViewport;
		}
	}
	return FEdMode::CapturedMouseMove(InViewportClient, InViewport, InMouseX, InMouseY);
}

bool FWFCEdModeActorPicker::LostFocus(FEditorViewportClient* ViewportClient, FViewport* Viewport)
{
	//UE_LOG(LogTemp, Warning, TEXT("Lost Focus!"));

	//if (ViewportClient == GCurrentLevelEditingViewportClient)
	if (ViewportClient)
	{
		// Make sure actor picking mode is disabled once the active viewport loses focus
		//RequestDeletion();
		return true;
	}

	return false;
}

bool FWFCEdModeActorPicker::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	//UE_LOG(LogTemp, Warning, TEXT("Input Key!"));

	//if (ViewportClient == GCurrentLevelEditingViewportClient)
	if (ViewportClient)
	{
		if (Key == EKeys::LeftMouseButton && Event == IE_Pressed)
		{
			int32 HitX = Viewport->GetMouseX();
			int32 HitY = Viewport->GetMouseY();
			HHitProxy* HitProxy = Viewport->GetHitProxy(HitX, HitY);
			if (HitProxy != NULL && HitProxy->IsA(HActor::StaticGetType()))
			{
				HActor* ActorHit = static_cast<HActor*>(HitProxy);
				if(ActorHit->Actor != NULL)
				{
					AActor* Actor = ActorHit->Actor;
					while (Actor->IsChildActor())
					{
						Actor = Actor->GetParentActor();
					}
					if(IsActorValid(Actor))
					{
						OnActorSelected.ExecuteIfBound(Actor);
					}
				}
			}
			bMouseButtonDown = true;
			return true;
		}
		else if (Key == EKeys::LeftMouseButton && Event == IE_Released)
		{
			bMouseButtonDown = false;
			return true;
		}
		else if (Key == EKeys::LeftShift && Event == IE_Pressed)
		{
			bShiftButtonDown = true;
			return true;
		}
		else if (Key == EKeys::LeftShift && Event == IE_Released)
		{
			bShiftButtonDown = false;
			return true;
		}
		else if (Key == EKeys::LeftAlt && Event == IE_Pressed)
		{
			bAltButtonDown = true;
			return true;
		}
		else if (Key == EKeys::LeftAlt && Event == IE_Released)
		{
			bAltButtonDown = false;
			return true;
		}
		else if(Key == EKeys::Escape && Event == IE_Pressed)
		{
			//RequestDeletion();
			return true;
		}
	}
	else
	{
		//RequestDeletion();
	}

	return false;
}

bool FWFCEdModeActorPicker::GetCursor(EMouseCursor::Type& OutCursor) const
{
	// if(HoveredActor.IsValid() && PickState == EPickState::OverActor)
	// {
	// 	OutCursor = EMouseCursor::EyeDropper;
	// }
	// else
	// {
	// 	OutCursor = EMouseCursor::SlashedCircle;
	// }
	OutCursor = EMouseCursor::Crosshairs;
	return true;
}

bool FWFCEdModeActorPicker::UsesToolkits() const
{
	return false;
}

bool FWFCEdModeActorPicker::IsCompatibleWith(FEditorModeID OtherModeID) const
{
	// We want to be able to perform this action with all the built-in editor modes
	return OtherModeID != FBuiltinEditorModes::EM_None;
}

void FWFCEdModeActorPicker::Exit()
{
	OnActorFilled = FOnActorFilled();
	OnActorSelected = FOnActorSelected();
	OnGetAllowedClasses = FOnGetAllowedClasses();
	OnShouldFilterActor = FOnShouldFilterActor();

	if (CursorDecoratorWindow.IsValid())
	{
		CursorDecoratorWindow->RequestDestroyWindow();
		CursorDecoratorWindow.Reset();
	}

	HoveredActor.Reset();
	PickState = EPickState::NotOverViewport;

	FEdMode::Exit();
}

FText FWFCEdModeActorPicker::GetCursorDecoratorText() const
{
	switch(PickState)
	{
	default:
	case EPickState::NotOverViewport:
		return LOCTEXT("PickActor_NotOverViewport", "Pick an actor by clicking on it in the active level viewport");
	case EPickState::OverViewport:
		return LOCTEXT("PickActor_NotOverActor", "Pick an actor by clicking on it");
	case EPickState::OverIncompatibleActor:
		{
			if(HoveredActor.IsValid())
			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("Actor"), FText::FromString(HoveredActor.Get()->GetActorNameOrLabel()));
				return FText::Format(LOCTEXT("PickActor_OverIncompatibleActor", "{Actor} is incompatible"), Arguments);
			}
			else
			{
				return LOCTEXT("PickActor_NotOverActor", "Pick an actor by clicking on it");
			}
		}
	case EPickState::OverActor:
		{
			if(HoveredActor.IsValid())
			{
				FFormatNamedArguments Arguments;
				Arguments.Add(TEXT("Actor"), FText::FromString(HoveredActor.Get()->GetActorNameOrLabel()));
				return FText::Format(LOCTEXT("PickActor_OverActor", "Pick {Actor}"), Arguments);
			}
			else
			{
				return LOCTEXT("PickActor_NotOverActor", "Pick an actor by clicking on it");
			}
		}
	}
}

bool FWFCEdModeActorPicker::IsActorValid(const AActor *const Actor) const
{
	bool bIsValid = false;

	if(Actor)
	{
		bool bHasValidClass = true;
		if(OnGetAllowedClasses.IsBound())
		{
			bHasValidClass = false;

			TArray<const UClass*> AllowedClasses;
			OnGetAllowedClasses.Execute(AllowedClasses);
			for(const UClass* AllowedClass : AllowedClasses)
			{
				if ((AllowedClass->IsChildOf(UInterface::StaticClass()) && Actor->GetClass()->ImplementsInterface(AllowedClass)) ||
					Actor->IsA(AllowedClass))
				{
					bHasValidClass = true;
					break;
				}
			}
		}

		bool bHasValidActor = true;
		if(OnShouldFilterActor.IsBound())
		{
			bHasValidActor = OnShouldFilterActor.Execute(Actor);
		}

		bIsValid = bHasValidClass && bHasValidActor;
	}

	return bIsValid;
}

#undef LOCTEXT_NAMESPACE
