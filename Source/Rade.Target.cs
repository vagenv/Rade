// Copyright 2015-2016 Vagen Ayrapetyan

using UnrealBuildTool;
using System.Collections.Generic;

public class RadeTarget : TargetRules
{
	public RadeTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
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
