// Copyright 2015-2017 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;

public class RadeTarget : TargetRules
{
   public RadeTarget(TargetInfo Target) : base(Target)
   {
      Type = TargetType.Game;
      DefaultBuildSettings = BuildSettingsVersion.V2;
      ExtraModuleNames.Add("Rade");
   }
}
