// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RCharacterLib : ModuleRules
{
   public RCharacterLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RUtilLib",
         "RSaveLib",
         "RInventoryLib"
      });
   }
}
