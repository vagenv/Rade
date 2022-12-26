// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RadeCharacter : ModuleRules
{
   public RadeCharacter(ReadOnlyTargetRules Target) : base(Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "RadeUtil",
         "RadeSave",
         "RadeInventory"
      });
   }
}
