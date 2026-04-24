

#pragma once

#include "CoreMinimal.h"
#include "SAS_PathDistanceCache.generated.h"

/**
 * 
 */
USTRUCT()
struct FSAS_PathDistanceCache
{
	GENERATED_BODY()

	UPROPERTY()
	float PathLength = 0.f;

	UPROPERTY()
	float LastValidatedTime = 0.f;
};