

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "SAS_SB_StorageDepot.generated.h"

class USAS_IC_ResourceDepot;
/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_SB_StorageDepot : public ASAS_SelectableBuilding
{
	GENERATED_BODY()
	
public:
	ASAS_SB_StorageDepot();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	USAS_IC_ResourceDepot* Inventory;


};
