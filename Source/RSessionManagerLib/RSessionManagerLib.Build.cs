// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RadeSessionManager : ModuleRules
{
   public RadeSessionManager (ReadOnlyTargetRules Target) : base(Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange(new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "HTTP",
         "OnlineSubsystem",
         "OnlineSubsystemUtils",
         "RadeUtil"
      });
      PrivateDependencyModuleNames.Add("OnlineSubsystem");
      DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
   }
}
