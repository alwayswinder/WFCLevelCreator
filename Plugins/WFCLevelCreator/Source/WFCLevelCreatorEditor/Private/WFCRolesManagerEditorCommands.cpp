#include "WFCRolesManagerEditorCommands.h"

#define LOCTEXT_NAMESPACE "WFCRolesManagerEditorCommands"

void FWFCRolesManagerEditorCommands::RegisterCommands()
{
	UI_COMMAND(ShowGrid, "ShowGrid", "Show WFC Grid Button Press", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(ShowDebug, "ShowDebug", "Show Debug Info Button Press", EUserInterfaceActionType::ToggleButton, FInputChord());
}
#undef LOCTEXT_NAMESPACE
