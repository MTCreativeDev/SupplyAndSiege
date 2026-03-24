// Copyright Jshaun Hookumchand. All Rights Reserved.

#include "HookLogBlueprintLibrary.h"
#include "HookDebugLog.h"

static void ExtractBlueprintContext(FFrame& Stack, FString& OutClass, FString& OutFunction)
{
	OutClass = Stack.Object ? Stack.Object->GetClass()->GetName() : TEXT("Unknown");
	OutFunction = Stack.Node ? Stack.Node->GetName() : TEXT("Unknown");
}

DEFINE_FUNCTION(UHookLogBlueprintLibrary::execHookLog)
{
	P_GET_ENUM(EHookLogType, Type);
	P_GET_PROPERTY(FStrProperty, Message);
	P_FINISH;

	FString ClassName;
	FString FunctionName;
	ExtractBlueprintContext(Stack, ClassName, FunctionName);

	HookDebugLog(Type, Message, ClassName, FunctionName, 0);
}

DEFINE_FUNCTION(UHookLogBlueprintLibrary::execHookLogQuiet)
{
	P_FINISH;

	FString ClassName;
	FString FunctionName;
	ExtractBlueprintContext(Stack, ClassName, FunctionName);

	HookDebugLog(EHookLogType::Info, TEXT(""), ClassName, FunctionName, 0, -1.f, FColor::Transparent, false);
}

void UHookLogBlueprintLibrary::HookLog(EHookLogType Type, const FString& Message)
{
	checkNoEntry();
}

void UHookLogBlueprintLibrary::HookLogQuiet()
{
	checkNoEntry();
}
