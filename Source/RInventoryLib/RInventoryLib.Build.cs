// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class RInventoryLib : ModuleRules
{
   public RInventoryLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "Json",
         "JsonUtilities",
         "RUtilLib",
         "RSaveLib"
      });
   }
}

