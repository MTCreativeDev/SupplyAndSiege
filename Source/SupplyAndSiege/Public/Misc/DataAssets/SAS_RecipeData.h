

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_GameDataAsset.h"
#include "SAS_RecipeData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_RecipeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TObjectPtr<USAS_GameDataAsset> Product;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TArray<TObjectPtr<USAS_GameDataAsset>> Requirements;
	
};
