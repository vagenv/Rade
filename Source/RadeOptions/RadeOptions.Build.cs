// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RadeOptions: ModuleRules
{
   public RadeOptions(ReadOnlyTargetRules Target) : base(Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "InputCore",
         "RHI",
         "RadeUtil"
      });
   }
}
