// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;
public class RExperienceLib: ModuleRules
{
   public RExperienceLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RUtilLib",
         "RSaveLib",
         "RDamageLib",
      });
   }
}

