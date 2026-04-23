

#pragma once

#include "CoreMinimal.h"
#include "SAS_LogisticsOfferingGrouped.generated.h"

class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct FSAS_LogisticsOfferingEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UItemDefinitionPrimaryData> Item = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity = 0;
};