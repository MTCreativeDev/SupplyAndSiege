

#pragma once

#include "CoreMinimal.h"
#include "SAS_IslandHealth.generated.h"

class USAS_IslandDefinitionData;

USTRUCT(BlueprintType)
struct FSAS_IslandHealth
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Island_Health")
	TMap<TObjectPtr<USAS_IslandDefinitionData>, int32> Health;

};
