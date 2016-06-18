// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Rade : ModuleRules
{
	public Rade(TargetInfo Target)
	{
        // Faster Build Speed
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HTTP", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "RHI" });

        PrivateDependencyModuleNames.Add("OnlineSubsystem");
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");

	}
}
