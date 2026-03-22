

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_BuildingLayout.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_BL_BuildJob.generated.h"

class USAS_UnitInformationComponent;
class USAS_InventoryComponent;
class USAS_InventoryProfileData;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_BL_BuildJob : public ASAS_BuildingLayout
{
	GENERATED_BODY()
	
public:
	ASAS_BL_BuildJob();

	void InitializeBuildJob(ESAS_Team NewAssignedTeam);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Construction")
	USAS_InventoryProfileData* ConstructionInventoryProfile;

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Unit")
	USAS_UnitInformationComponent* UnitInformation;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	USAS_InventoryComponent* Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Construction")
	TObjectPtr<UMaterialInterface> BuildingUnderConstructionMaterial;

};
