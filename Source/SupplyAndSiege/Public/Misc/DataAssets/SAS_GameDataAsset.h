

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_GameDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_GameDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
    FText Description;

};
