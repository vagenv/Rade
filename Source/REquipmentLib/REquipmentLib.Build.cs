// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;

public class REquipmentLib : ModuleRules
{
   public REquipmentLib (ReadOnlyTargetRules Target) : base (Target)
   {
      PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
      PublicDependencyModuleNames.AddRange (new string[] {
         "Core",
         "CoreUObject",
         "Engine",
         "Json",
         "JsonUtilities",
         "RUtilLib",
         "RUILib",
         "RSaveLib",
         "RDamageLib",
         "RInventoryLib",
         "RStatusLib"
      });
   }
}

