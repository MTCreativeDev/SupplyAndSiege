// Copyright Jshaun Hookumchand. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Misc/Paths.h"

// Force macro expansion before DECLARE_LOG_CATEGORY_EXTERN stringifies the token
#define HOOK_DECLARE_LOG_CAT(Name) DECLARE_LOG_CATEGORY_EXTERN(Name, Log, All)
HOOK_DECLARE_LOG_CAT(HOOK_LOG_CATEGORY);

UENUM(BlueprintType)
enum class EHookLogType : uint8
{
	Success,   // Green:  default 4s
	Warning,   // Yellow: default 4s
	Error,     // Red:    default 4s
	Info       // White:  default 4s
};

/*
Global debug log function.
Always writes to UE_LOG with format: [File:Line][Func] Message
On-screen messages: on by default in editor, off by default in non-editor, never in shipping.
Controlled via hook.DebugScreenLog CVar or Editor Preferences -> Debug Settings.
Uses enum default colour unless ColorOverride != FColor::Transparent.
Uses enum default duration (4s) unless Duration >= 0.

Console variables (set in console or DefaultEngine.ini):
  hook.DebugScreenLog:      enable on-screen messages (0/1). Default: 1 in editor, 0 otherwise. Stripped in shipping.
  hook.ScreenShowFile:      include filename in on-screen messages (0/1)
  hook.ScreenShowFunction:  include function name in on-screen messages (0/1)
  hook.ScreenShowLine:      include line number in on-screen messages (0/1)
*/
HOOKDEBUGLOG_API void HookDebugLog(
	EHookLogType Type,
	const FString& Message,
	const FString& File,
	const FString& Func,
	int32 Line,
	float Duration = -1.f,
	FColor ColorOverride = FColor::Transparent,
	bool bPrintToScreen = true);

// Log with automatic file/function/line injection. Color and Duration are optional.
#define HOOK_LOG(Type, Message, ...) \
	HookDebugLog(Type, Message, TEXT(__FILE__), TEXT(__FUNCTION__), __LINE__, ##__VA_ARGS__)

// Log only to output log with no message, no screen print. Breadcrumb that records file/function/line.
#define HOOK_LOG_QUIET() \
	HookDebugLog(EHookLogType::Info, TEXT(""), TEXT(__FILE__), TEXT(__FUNCTION__), __LINE__, -1.f, FColor::Transparent, false)
