// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DarkSouls : ModuleRules
{
	public DarkSouls(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "GameplayCameras" });

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
