// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RAbilityLib : ModuleRules
{
   public RAbilityLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "Json",
         "JsonUtilities",
         "RUtilLib",
         "RSaveLib",
      });
   }
}
