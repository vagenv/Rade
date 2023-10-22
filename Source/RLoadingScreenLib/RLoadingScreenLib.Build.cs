// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RLoadingScreenLib : ModuleRules
{
   public RLoadingScreenLib (ReadOnlyTargetRules Target) : base(Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "Slate",
         "SlateCore",
         "MoviePlayer",
         "DeveloperSettings",
         "RUtilLib"
      });
   }
}
