// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RSaveLib: ModuleRules
{
   public RSaveLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RUtilLib"
      });
   }
}
