// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RSessionManagerLib : ModuleRules
{
   public RSessionManagerLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "HTTP",
         "OnlineSubsystem",
         "OnlineSubsystemUtils",
         "RUtilLib"
      });
      PrivateDependencyModuleNames.Add ("OnlineSubsystem");
      DynamicallyLoadedModuleNames.Add ("OnlineSubsystemNull");
   }
}
