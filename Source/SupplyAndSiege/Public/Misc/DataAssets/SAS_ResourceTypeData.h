
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "SAS_ResourceTypeData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_ResourceTypeData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Resource")
	FName DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	int32 StartingAmount = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TObjectPtr<UItemDefinitionPrimaryData> YieldItemDefinition = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TSubclassOf<class ASAS_SB_ResourceDepot> ResourceDepot = nullptr;
		
};
