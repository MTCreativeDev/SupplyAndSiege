// Copyright Jshaun Hookumchand. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HookDebugLog.h"
#include "HookLogBlueprintLibrary.generated.h"

UCLASS()
class HOOKDEBUGLOG_API UHookLogBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*
	Blueprint equivalent of HOOK_LOG.
	Automatically extracts the calling Blueprint class and graph/function name.
	On-screen display respects the same CVars and Debug Settings as C++ HOOK_LOG.
	*/
	UFUNCTION(BlueprintCallable, Category = "Hook|Debug", CustomThunk)
	static void HookLog(EHookLogType Type, const FString& Message);
	DECLARE_FUNCTION(execHookLog);

	// Blueprint equivalent of HOOK_LOG_QUIET. Logs to output only with no message.
	UFUNCTION(BlueprintCallable, Category = "Hook|Debug", CustomThunk)
	static void HookLogQuiet();
	DECLARE_FUNCTION(execHookLogQuiet);

};
