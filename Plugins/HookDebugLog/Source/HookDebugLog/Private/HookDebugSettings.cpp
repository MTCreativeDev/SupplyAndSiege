// Copyright Jshaun Hookumchand. All Rights Reserved.

#include "HookDebugSettings.h"
#include "HAL/IConsoleManager.h"

UHookDebugSettings::UHookDebugSettings()
{
}

void UHookDebugSettings::PostInitProperties()
{
	Super::PostInitProperties();
	SyncToCVars();
}

FName UHookDebugSettings::GetCategoryName() const
{
	return FName(FApp::GetProjectName());
}

FName UHookDebugSettings::GetSectionName() const
{
	return FName(TEXT("Debug Settings"));
}

#if WITH_EDITOR
void UHookDebugSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SyncToCVars();
}
#endif

UHookDebugSettings* UHookDebugSettings::Get()
{
	return GetMutableDefault<UHookDebugSettings>();
}

void UHookDebugSettings::SyncToCVars()
{
	auto SetCVar = [](const TCHAR* Name, bool bValue)
	{
		if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
		{
			CVar->Set(bValue ? 1 : 0, ECVF_SetByConsole);
		}
	};

	SetCVar(TEXT("hook.DebugScreenLog"),    bScreenLog);
	SetCVar(TEXT("hook.ScreenShowFile"),     bShowFile);
	SetCVar(TEXT("hook.ScreenShowFunction"), bShowFunction);
	SetCVar(TEXT("hook.ScreenShowLine"),     bShowLine);
}
