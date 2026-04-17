

#pragma once

#include "CoreMinimal.h"
#include "SAS_IslandCaelium.generated.h"

class USAS_IslandDefinitionData;

USTRUCT(BlueprintType)
struct FSAS_IslandCaelium
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Island_Caelium")
	TMap<TObjectPtr<USAS_IslandDefinitionData>, int32> Health;

};
