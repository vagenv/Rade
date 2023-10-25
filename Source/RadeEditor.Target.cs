// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;


public class RadeEditorTarget : TargetRules
{
   public RadeEditorTarget (TargetInfo Target) : base(Target)
   {
      Type = TargetType.Editor;
      bUsesSteam = true;
      DefaultBuildSettings = BuildSettingsVersion.V4;
      IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

      ExtraModuleNames.Add ("RUtilLib");
      ExtraModuleNames.Add ("RLoadingScreenLib");
      ExtraModuleNames.Add ("RInputLib");
      ExtraModuleNames.Add ("RDamageLib");
      ExtraModuleNames.Add ("RExperienceLib");
      ExtraModuleNames.Add ("RTargetLib");
      ExtraModuleNames.Add ("RInteractLib");
      ExtraModuleNames.Add ("RUILib");
      ExtraModuleNames.Add ("RSaveLib");
      ExtraModuleNames.Add ("ROptionsLib");
      ExtraModuleNames.Add ("RInventoryLib");
      ExtraModuleNames.Add ("RSessionLib");
      ExtraModuleNames.Add ("RStatusLib");
      ExtraModuleNames.Add ("RAbilityLib");
      ExtraModuleNames.Add ("REquipmentLib");
      ExtraModuleNames.Add ("Rade");

      ExtraModuleNames.Add ("RTypesEditor");
   }
}
