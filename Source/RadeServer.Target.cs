// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class RadeServerTarget : TargetRules
{
	public RadeServerTarget (TargetInfo Target) : base(Target)
	{
      Type = TargetType.Server;
      bUsesSteam = true;
      DefaultBuildSettings = BuildSettingsVersion.V2;
      IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
	  
      ExtraModuleNames.Add ("RUtilLib");
      ExtraModuleNames.Add ("RInputLib");
      ExtraModuleNames.Add ("RDamageLib");
      ExtraModuleNames.Add ("RExperienceLib");
      ExtraModuleNames.Add ("RTargetLib");
      ExtraModuleNames.Add ("RUILib");
      ExtraModuleNames.Add ("RSaveLib");
      ExtraModuleNames.Add ("ROptionsLib");
      ExtraModuleNames.Add ("RInventoryLib");
      ExtraModuleNames.Add ("RSessionLib");
      ExtraModuleNames.Add ("RStatusLib");
      ExtraModuleNames.Add ("RAbilityLib");
      ExtraModuleNames.Add ("REquipmentLib");
      ExtraModuleNames.Add ("RLoadingScreenLib");
      ExtraModuleNames.Add ("Rade");
	}
}
