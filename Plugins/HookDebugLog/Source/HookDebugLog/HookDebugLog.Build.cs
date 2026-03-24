// Copyright Jshaun Hookumchand. All Rights Reserved.

using UnrealBuildTool;

public class HookDebugLog : ModuleRules
{
	public HookDebugLog(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		string LogCategoryName = ReadLogCategoryFromConfig(Target);
		if (string.IsNullOrWhiteSpace(LogCategoryName))
		{
			LogCategoryName = Target.ProjectFile.GetFileNameWithoutExtension() + "Log";
		}
		PublicDefinitions.Add($"HOOK_LOG_CATEGORY={LogCategoryName}");

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		PrivateDependencyModuleNames.AddRange(new string[] { "DeveloperSettings" });
	}

	private string ReadLogCategoryFromConfig(ReadOnlyTargetRules Target)
	{
		string ConfigPath = System.IO.Path.Combine(
			Target.ProjectFile.Directory.FullName, "Config", "DefaultEngine.ini");

		if (!System.IO.File.Exists(ConfigPath))
		{
			return null;
		}

		bool bInSection = false;
		foreach (string Line in System.IO.File.ReadLines(ConfigPath))
		{
			string Trimmed = Line.Trim();

			if (Trimmed.StartsWith("["))
			{
				bInSection = Trimmed == "[/Script/HookDebugLog.HookDebugLogConfig]";
				continue;
			}

			if (bInSection && Trimmed.StartsWith("LogCategoryName="))
			{
				return Trimmed.Substring("LogCategoryName=".Length).Trim();
			}
		}

		return null;
	}
}
