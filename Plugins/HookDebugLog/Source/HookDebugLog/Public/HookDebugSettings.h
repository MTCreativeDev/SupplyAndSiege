// Copyright Jshaun Hookumchand. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HookDebugSettings.generated.h"

/*
Changes made in the panel are synced to the console variables immediately.
You can also use the console directly (hook.DebugScreenLog, etc.).
*/
UCLASS(Config=EditorPerProjectUserSettings,
       DefaultConfig,
       meta=(DisplayName="Debug Settings"))
class HOOKDEBUGLOG_API UHookDebugSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UHookDebugSettings();

	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName()  const override;

	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(Config, EditAnywhere, Category="Screen Output",
	          meta=(DisplayName="Enable On-Screen Messages"))
	bool bScreenLog = false;

	UPROPERTY(Config, EditAnywhere, Category="Screen Output",
	          meta=(DisplayName="Show File Name"))
	bool bShowFile = true;

	UPROPERTY(Config, EditAnywhere, Category="Screen Output",
	          meta=(DisplayName="Show Function Name"))
	bool bShowFunction = true;

	UPROPERTY(Config, EditAnywhere, Category="Screen Output",
	          meta=(DisplayName="Show Line Number"))
	bool bShowLine = true;

	static UHookDebugSettings* Get();

private:

	void SyncToCVars();
};
