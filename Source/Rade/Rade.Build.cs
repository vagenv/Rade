// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Rade : ModuleRules
{
   public Rade (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "InputCore",
         "RUtilLib",
         "RSaveLib",
         "RCharacterLib"
      });
   }
}
