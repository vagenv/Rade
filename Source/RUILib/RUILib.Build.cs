// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RUILib : ModuleRules
{
   public RUILib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RHI",
         "RenderCore",
         "RUtilLib",
      });
   }
}

