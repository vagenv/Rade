// Copyright 2015-2023 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;


public class RadeClientTarget : TargetRules
{
	public RadeClientTarget (TargetInfo Target) : base(Target)
	{	
		Type = TargetType.Client;
      bUsesSteam = true;
      DefaultBuildSettings = BuildSettingsVersion.V2;
      IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
	  
      ExtraModuleNames.Add ("RUtilLib");
      ExtraModuleNames.Add ("RDamageLib");
      ExtraModuleNames.Add ("RExperienceLib");
      ExtraModuleNames.Add ("RTargetableLib");
      ExtraModuleNames.Add ("RUILib");
      ExtraModuleNames.Add ("RSaveLib");
      ExtraModuleNames.Add ("ROptionsLib");
      ExtraModuleNames.Add ("RInventoryLib");
      ExtraModuleNames.Add ("RSessionManagerLib");
      ExtraModuleNames.Add ("RStatusLib");
      ExtraModuleNames.Add ("RAbilityLib");
      ExtraModuleNames.Add ("REquipmentLib");
      ExtraModuleNames.Add ("RLoadingScreenLib");
      ExtraModuleNames.Add ("Rade");
   }
}
