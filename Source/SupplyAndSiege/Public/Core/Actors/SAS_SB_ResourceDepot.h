

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "Core/Interfaces/SAS_BuildingWithInventory.h"
#include "SAS_SB_ResourceDepot.generated.h"


class USAS_IC_ResourceDepot;
/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_SB_ResourceDepot : public ASAS_SelectableBuilding, public ISAS_BuildingWithInventory
{
	GENERATED_BODY()

public:
	ASAS_SB_ResourceDepot();

	virtual USAS_InventoryComponent* GetInputInventory() override;

protected:



protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	USAS_IC_ResourceDepot* Inventory;
		
};
