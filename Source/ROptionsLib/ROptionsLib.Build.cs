// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;
public class ROptionsLib: ModuleRules
{
   public ROptionsLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "InputCore",
         "RHI",
         "RUtilLib"
      });
   }
}

