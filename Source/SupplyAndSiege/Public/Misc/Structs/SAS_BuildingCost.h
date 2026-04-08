

#pragma once

#include "CoreMinimal.h"
#include "SAS_BuildingCost.generated.h"

class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct FSAS_BuildingCost
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Cost")
	TMap<TObjectPtr<UItemDefinitionPrimaryData>, int32> ResourceCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Cost")
	float ConstructionWorkRequired = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Cost")
	int32 MaxConstructionWorkers = 5;

};