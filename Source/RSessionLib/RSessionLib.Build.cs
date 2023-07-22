// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RSessionLib : ModuleRules
{
   public RSessionLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "EngineSettings",
         "HTTP",
         "OnlineSubsystem",
         "OnlineSubsystemUtils",
         "RUtilLib"
      });
      PrivateDependencyModuleNames.Add ("OnlineSubsystem");
      DynamicallyLoadedModuleNames.Add ("OnlineSubsystemNull");
   }
}

