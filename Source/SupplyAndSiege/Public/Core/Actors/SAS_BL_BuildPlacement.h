

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_BuildingLayout.h"
#include "SAS_BL_BuildPlacement.generated.h"

/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_BL_BuildPlacement : public ASAS_BuildingLayout
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	virtual void ApplyDefinitionToComponents() override;

	bool GetIsValidPlacement() const { return bIsValidPlacement;}

	void CheckPlacementValidity();

protected:

private:
	void UpdateMeshMaterials();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Placement")
	TObjectPtr<UMaterialInterface> ValidPlacementMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Placement")
	TObjectPtr<UMaterialInterface> InvalidPlacementMaterial;




private:
	bool bIsValidPlacement = false;

	//TODO: Need a better solution to this. This is the lazy way of making sure it updates in situations where the first check doesnt properly update materials;
	bool bHasNotUpdatedMaterial = true;



};
