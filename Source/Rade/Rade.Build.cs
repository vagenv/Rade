// Copyright 2015-2023 Vagen Ayrapetyan

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
         "EnhancedInput",
         "RUtilLib",
         "ROptionsLib",
         "RSessionManagerLib",
         "RSaveLib",
         "RInventoryLib",
         "RStatusLib",
         "REquipmentLib",
      });
   }
}

