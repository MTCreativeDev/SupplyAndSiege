

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/Structs/SAS_BuildingCost.h"
#include "SAS_BuildingDefinitionData.generated.h"

class UStaticMesh;
/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API USAS_BuildingDefinitionData : public UPrimaryDataAsset
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
	FVector BuildingAreaCollisionExtents = FVector(800.f, 800.f, 200.f);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector BuildingAreaCollisionLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> PrimaryMesh = nullptr;;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> SecondaryMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform PrimaryMeshRelativeTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform SecondaryMeshRelativeTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FVector> MoveToRelativeLocations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSAS_BuildingCost BuildingCost;

};
