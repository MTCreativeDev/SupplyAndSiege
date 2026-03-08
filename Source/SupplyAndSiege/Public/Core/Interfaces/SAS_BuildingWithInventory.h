

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SAS_BuildingWithInventory.generated.h"

class USAS_InventoryComponent;

UINTERFACE(BlueprintType, NotBlueprintable)
class USAS_BuildingWithInventory : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SUPPLYANDSIEGE_API ISAS_BuildingWithInventory
{
	GENERATED_BODY()

	
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual USAS_InventoryComponent* GetInputInventory() = 0;

};
