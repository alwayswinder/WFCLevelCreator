// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WFCLevelCreatorEditor : ModuleRules
{
	public WFCLevelCreatorEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				"WFCLevelCreator/Public",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AssetTools",
				// ... add other public dependencies that you statically link with here ...
				"WFCLevelCreator",
				"UnrealEd",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
				"AdvancedPreviewScene",
				"EditorFramework",
				"ContentBrowser",
				"InputCore",
				"Projects",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
