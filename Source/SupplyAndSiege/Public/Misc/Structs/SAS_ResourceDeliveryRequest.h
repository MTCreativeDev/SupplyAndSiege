

#pragma once

#include "CoreMinimal.h"
#include "SAS_ResourceDeliveryRequest.generated.h"

class ASAS_BL_BuildJob;
class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct FSAS_ResourceDeliveryRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	TObjectPtr<ASAS_BL_BuildJob> BuildSite = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	TMap<TObjectPtr<UItemDefinitionPrimaryData>, int32> ResourceCost;

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	int32 Priority = 1;

};