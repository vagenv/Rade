// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Rade : ModuleRules
{
   public Rade(ReadOnlyTargetRules Target) : base(Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      // Faster Build Speed
      //MinFilesUsingPrecompiledHeaderOverride = 1;
      //bUseUnity = false;

      PublicDependencyModuleNames.AddRange(new string[] {
         "Core", "CoreUObject", "Engine", "InputCore",
         "HTTP", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "RHI"
      });
      PrivateDependencyModuleNames.Add("OnlineSubsystem");
      DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
   }
}
