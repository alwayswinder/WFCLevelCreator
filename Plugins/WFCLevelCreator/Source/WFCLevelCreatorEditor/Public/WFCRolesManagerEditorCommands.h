#pragma once
#include "FWFCUIStyle.h"

class FWFCRolesManagerEditorCommands : public TCommands<FWFCRolesManagerEditorCommands>
{
public:
	FWFCRolesManagerEditorCommands() : TCommands<FWFCRolesManagerEditorCommands>(
		"WFCRolesManagerEditor",
		NSLOCTEXT("WFCRolesManagerEditor", "WFC Roles Manager Editor", "WFC Roles Manager Editor Commands"),
		"EditorViewport",
		FWFCUIStyle::GetStyleSetName())
		{}

	TSharedPtr<FUICommandInfo> ShowGrid;

	TSharedPtr<FUICommandInfo> FillGrid;

	/** Register commands */
	virtual void RegisterCommands() override;
};
