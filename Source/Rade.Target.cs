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
      ExtraModuleNames.Add ("RadeUtil");
      ExtraModuleNames.Add ("RadeSave");
      ExtraModuleNames.Add ("RadeOptions");
      ExtraModuleNames.Add ("RadeInventory");
      ExtraModuleNames.Add ("RadeSessionManager");
      ExtraModuleNames.Add ("RadeCharacter");
      ExtraModuleNames.Add ("Rade");
   }
}
