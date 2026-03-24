// Copyright Jshaun Hookumchand. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HookDebugLogConfig.generated.h"

/*
Project-wide log category name. Stored in DefaultEngine.ini.
Requires a recompile to take effect.
*/
UCLASS(Config=Engine, DefaultConfig, meta=(DisplayName="Hook Debug Log"))
class HOOKDEBUGLOG_API UHookDebugLogConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category="Log Category",
	          meta=(DisplayName="Log Category Name",
	                ConfigRestartRequired=true,
	                ToolTip="Override the UE_LOG category name. Leave blank to use {ProjectName}Log. Requires a recompile to take effect."))
	FString LogCategoryName;

	virtual FName GetCategoryName() const override;
};
