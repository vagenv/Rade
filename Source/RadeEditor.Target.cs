// Copyright 2015-2021 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;

public class RadeEditorTarget : TargetRules
{
   public RadeEditorTarget(TargetInfo Target) : base(Target)
   {
      Type = TargetType.Editor;
      DefaultBuildSettings = BuildSettingsVersion.V2;
      ExtraModuleNames.Add("Rade");
   }
}
