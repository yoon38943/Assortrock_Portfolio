// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;
using System.Collections.Generic;

public class WillBeAOS : ModuleRules
{
	public WillBeAOS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara", "OnlineSubsystem", "OnlineSubsystemSteam" });

		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule", "NavigationSystem" });

        // ���� ��� �߰�
        PublicIncludePaths.Add(ModuleDirectory);
        PublicIncludePaths.Add("WillBeAOS");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
