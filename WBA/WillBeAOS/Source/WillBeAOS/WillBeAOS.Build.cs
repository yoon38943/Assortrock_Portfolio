// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class WillBeAOS : ModuleRules
{
	public WillBeAOS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Niagara", "OnlineSubsystem", "OnlineSubsystemSteam" });

		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });

        // ���� ��� �߰�
        PublicIncludePaths.Add(ModuleDirectory);
        PublicIncludePaths.Add("WillBeAOS");
        
        string SDKPath = Path.Combine(ModuleDirectory, "../../sdk");
        string SDKPublicIncludePath = Path.Combine(SDKPath, "public");
        string SDKLibFile = Path.Combine(SDKPath, "redistributable_bin", "win64", "steam_api64.lib");

        PublicIncludePaths.Add(SDKPublicIncludePath);
        PublicAdditionalLibraries.Add(SDKLibFile);

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
