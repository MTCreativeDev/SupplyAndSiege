#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_DamageTypeData.generated.h"

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_DamageTypeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage Type")
	FText DisplayName;
};
