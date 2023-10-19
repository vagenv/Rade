// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;
public class RInteractLib: ModuleRules
{
   public RInteractLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RUtilLib"
      });
   }
}

