// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RUtilLib : ModuleRules
{
   public RUtilLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine"
      });
   }
}

