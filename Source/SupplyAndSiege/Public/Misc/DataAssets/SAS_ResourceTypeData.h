
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "EnvironmentQuery/EnvQuery.h"
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
	UEnvQuery* ResourceIsReachableQuery;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TArray<FVector> ResourceIsReachableQueryLocations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	UEnvQuery* LikeKindResourceQuery;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	float LikeKindSearchRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	UEnvQuery* ResourceDepotQuery;
		
};
