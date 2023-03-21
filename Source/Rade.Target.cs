// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RadeTarget : TargetRules
{
   public RadeTarget (TargetInfo Target) : base(Target)
   {
      Type = TargetType.Game;
      DefaultBuildSettings = BuildSettingsVersion.V2;
      IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
      ExtraModuleNames.Add ("RUtilLib");
      ExtraModuleNames.Add ("RDamageLib");
      ExtraModuleNames.Add ("RExperienceLib");
      ExtraModuleNames.Add ("RTargetableLib");
      ExtraModuleNames.Add ("RSaveLib");
      ExtraModuleNames.Add ("ROptionsLib");
      ExtraModuleNames.Add ("RInventoryLib");
      ExtraModuleNames.Add ("RSessionManagerLib");
      ExtraModuleNames.Add ("RStatusLib");
      ExtraModuleNames.Add ("RAbilityLib");
      ExtraModuleNames.Add ("REquipmentLib");
      ExtraModuleNames.Add ("Rade");
   }
}
