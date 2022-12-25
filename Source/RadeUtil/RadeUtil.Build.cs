// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RadeUtil : ModuleRules
{
   public RadeUtil(ReadOnlyTargetRules Target) : base(Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine"
      });
   }
}
