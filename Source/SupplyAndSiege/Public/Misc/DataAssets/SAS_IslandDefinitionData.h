

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/Structs/SAS_IslandHealth.h"
#include "SAS_IslandDefinitionData.generated.h"

class UStaticMesh;
class UTexture2D;

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_IslandDefinitionData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	public:

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText DisplayName;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText Description;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UTexture2D* Icon;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FSAS_IslandHealth IslandHealth;

		// TODO - Update FVector values with real numbers
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FVector IslandAreaCollisionExtents = FVector(0.f, 0.f, 0.f);

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FVector IslandAreaCollisionLocation = FVector::ZeroVector;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TObjectPtr<UStaticMesh> IslandMesh = nullptr;;

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FTransform IslandMeshRelativeTransform = FTransform::Identity;
};
