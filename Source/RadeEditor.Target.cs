// Copyright 2015 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;

public class RadeEditorTarget : TargetRules
{
	public RadeEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "Rade" } );
	}
}
