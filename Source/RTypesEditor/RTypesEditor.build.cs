// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RTypesEditor : ModuleRules
{
	public RTypesEditor (ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RUtilLib",
			"RUILib",
			"RStatusLib",
			"RExperienceLib",
			"RInventoryLib",
			"Rade",

			"InputCore",
			"Slate",
			"SlateCore",
			"UnrealEd",
			"PropertyEditor",
			"EditorStyle"
      });
	}
}
