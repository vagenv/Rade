// Copyright 2015-2017 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;

public class RadeEditorTarget : TargetRules
{
	public RadeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Rade" } );
	}
}
