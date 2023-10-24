using UnrealBuildTool;

public class RadeEditor : ModuleRules
{
	public RadeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RUtilLib",
			"RUILib",
			"RStatusLib",
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
