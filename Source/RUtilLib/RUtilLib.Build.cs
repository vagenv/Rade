// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RUtilLib : ModuleRules
{
   public RUtilLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine"
      });
   }
}
