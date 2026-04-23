

#pragma once

#include "CoreMinimal.h"
#include "SAS_InventoryPriorityThreshold.generated.h"


USTRUCT(BlueprintType)
struct FSAS_InventoryPriorityThreshold
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MinimumQuantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Priority = 1;
};