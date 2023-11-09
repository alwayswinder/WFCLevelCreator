// Fill out your copyright notice in the Description page of Project Settings.


#include "FWFCUIStyle.h"

#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"
#include "Styling/AppStyle.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define IMAGE_BRUSH_SVG(RelativePath, ...) FSlateVectorImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

TSharedPtr< FSlateStyleSet > FWFCUIStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FWFCUIStyle::Get() { return StyleSet; }

FName FWFCUIStyle::GetStyleSetName()
{
	static FName WFCStyleName(TEXT("WFCUIStyle"));
	return WFCStyleName;
}

void FWFCUIStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("WFCLevelCreator"))->GetContentDir());
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	// WaterWaves Asset Editor
	StyleSet->Set("WFCRolesManagerEditor.ShowGrid", new IMAGE_BRUSH("Icons/icon_ExtractSprites_40x", Icon40x40));
	StyleSet->Set("WFCRolesManagerEditor.ShowDebug", new IMAGE_BRUSH("Icons/WaterLandscapeBrush_x256", Icon40x40));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

#undef IMAGE_BRUSH_SVG
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef DEFAULT_FONT

void FWFCUIStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}
