// Copyright Jshaun Hookumchand. All Rights Reserved.

#include "HookDebugLog.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, HookDebugLog);

#define HOOK_DEFINE_LOG_CAT(Name) DEFINE_LOG_CATEGORY(Name)
HOOK_DEFINE_LOG_CAT(HOOK_LOG_CATEGORY);

#if WITH_EDITOR
static TAutoConsoleVariable<int32> CVarHookScreenLog(TEXT("hook.DebugScreenLog"), 1, TEXT("Enable on-screen debug messages. 0=off 1=on"));
#elif !UE_BUILD_SHIPPING
static TAutoConsoleVariable<int32> CVarHookScreenLog(TEXT("hook.DebugScreenLog"), 0, TEXT("Enable on-screen debug messages. 0=off 1=on"));
#endif

static TAutoConsoleVariable<int32> CVarHookScreenShowFile(TEXT("hook.ScreenShowFile"), 1, TEXT("Include filename in on-screen messages. 0=off 1=on"));

static TAutoConsoleVariable<int32> CVarHookScreenShowFunction(TEXT("hook.ScreenShowFunction"), 1, TEXT("Include function name in on-screen messages. 0=off 1=on"));

static TAutoConsoleVariable<int32> CVarHookScreenShowLine(TEXT("hook.ScreenShowLine"), 1, TEXT("Include line number in on-screen messages. 0=off 1=on"));

static FString GetCleanFileName(const FString& File)
{
	return FPaths::GetCleanFilename(File);
}

static FString BuildLogPrefix(const FString& File, const FString& Func, int32 Line)
{
	if (Line > 0)
	{
		return FString::Printf(TEXT("[%s:%d][%s]"), *GetCleanFileName(File), Line, *Func);
	}
	return FString::Printf(TEXT("[%s][%s]"), *GetCleanFileName(File), *Func);
}

static FString BuildScreenPrefix(const FString& File, const FString& Func, int32 Line)
{
	const bool bShowFile     = CVarHookScreenShowFile.GetValueOnGameThread() != 0;
	const bool bShowFunction = CVarHookScreenShowFunction.GetValueOnGameThread() != 0;
	const bool bShowLine     = CVarHookScreenShowLine.GetValueOnGameThread() != 0;

	FString Prefix;

	if (bShowFile || bShowLine)
	{
		Prefix += TEXT("[");
		if (bShowFile)
		{
			Prefix += GetCleanFileName(File);
		}
		if (bShowLine && Line > 0)
		{
			if (bShowFile)
			{
				Prefix += TEXT(":");
			}
			Prefix += FString::Printf(TEXT("%d"), Line);
		}
		Prefix += TEXT("]");
	}

	if (bShowFunction)
	{
		Prefix += FString::Printf(TEXT("[%s]"), *Func);
	}

	return Prefix;
}

static FColor ResolveColor(EHookLogType Type, FColor ColorOverride)
{
	if (ColorOverride != FColor::Transparent)
	{
		return ColorOverride;
	}

	switch (Type)
	{
	case EHookLogType::Success: return FColor::Green;
	case EHookLogType::Warning: return FColor::Yellow;
	case EHookLogType::Error:   return FColor::Red;
	case EHookLogType::Info:
	default:                    return FColor::White;
	}
}

static float ResolveDuration(float Duration)
{
	return (Duration >= 0.f) ? Duration : 4.f;
}

static bool ShouldPrintToScreen()
{
#if UE_BUILD_SHIPPING
	return false;
#else
	return CVarHookScreenLog.GetValueOnGameThread() != 0;
#endif
}

static void WriteToLog(EHookLogType Type, const FString& LogPrefix, const FString& Message)
{
	switch (Type)
	{
	case EHookLogType::Warning:
		UE_LOG(HOOK_LOG_CATEGORY, Warning, TEXT("%s %s"), *LogPrefix, *Message);
		break;
	case EHookLogType::Error:
		UE_LOG(HOOK_LOG_CATEGORY, Error, TEXT("%s %s"), *LogPrefix, *Message);
		break;
	case EHookLogType::Success:
	case EHookLogType::Info:
	default:
		UE_LOG(HOOK_LOG_CATEGORY, Log, TEXT("%s %s"), *LogPrefix, *Message);
		break;
	}
}

static void PrintToScreen(EHookLogType Type, const FString& ScreenPrefix,
                           const FString& Message, float Duration, FColor ColorOverride)
{
	if (!GEngine)
	{
		return;
	}

	const FString ScreenMessage = ScreenPrefix.IsEmpty()
		? Message
		: FString::Printf(TEXT("%s %s"), *ScreenPrefix, *Message);

	GEngine->AddOnScreenDebugMessage(-1, ResolveDuration(Duration), ResolveColor(Type, ColorOverride), ScreenMessage);
}

void HookDebugLog(
	EHookLogType Type,
	const FString& Message,
	const FString& File,
	const FString& Func,
	int32 Line,
	float Duration,
	FColor ColorOverride,
	bool bPrintToScreen)
{
	const FString LogPrefix = BuildLogPrefix(File, Func, Line);

	WriteToLog(Type, LogPrefix, Message);

	if (bPrintToScreen && ShouldPrintToScreen())
	{
		const FString ScreenPrefix = BuildScreenPrefix(File, Func, Line);
		PrintToScreen(Type, ScreenPrefix, Message, Duration, ColorOverride);
	}
}
