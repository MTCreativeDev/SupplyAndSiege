// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SupplyAndSiege : ModuleRules
{
	public SupplyAndSiege(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // For HookDebugLog
        PrivatePCHHeaderFile = "SupplyAndSiege.h";
        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "AIModule", "HookDebugLog", "NavigationSystem"});

		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule", "StateTreeModule", "GameplayStateTreeModule" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
