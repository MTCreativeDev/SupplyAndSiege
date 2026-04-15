#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_HealthDefinitionData.generated.h"

class USAS_DamageTypeData;

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_HealthDefinitionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TMap<USAS_DamageTypeData*, float> DamageTypeResistances;
};
