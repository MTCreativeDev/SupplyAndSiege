// Copyright Jshaun Hookumchand. All Rights Reserved.

#include "HookDebugLogConfig.h"

FName UHookDebugLogConfig::GetCategoryName() const
{
	return FName(FApp::GetProjectName());
}
