// Fill out your copyright notice in the Description page of Project Settings.

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
