
#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_ResourceGenSelectableBuilding.h"
#include "Misc/DataAssets/SAS_GameDataAsset.h"
#include "SAS_ResourceDeliveryRequest2.generated.h"

class ASAS_BL_BuildJob;
class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct FSAS_ResourceDeliveryRequest2
{
	GENERATED_BODY()

	bool IsValid() const
	{
		if (!BuildSite) return false;
		if (ResourceCost.IsEmpty()) return false;
		for (const TPair<TObjectPtr<USAS_GameDataAsset>, int32>& Pair : ResourceCost)
		{
			if (!Pair.Key) return false;
			if (Pair.Value <= 0) return false;
		}
		return true;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	TObjectPtr<ASAS_ResourceGenSelectableBuilding> BuildSite = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	TMap<TObjectPtr<USAS_GameDataAsset>, int32> ResourceCost;

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	int32 Priority = 1;

};