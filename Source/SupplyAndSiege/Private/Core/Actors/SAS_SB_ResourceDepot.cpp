


#include "Core/Actors/SAS_SB_ResourceDepot.h"
#include "Core/Components/SAS_InventoryComponent.h"



ASAS_SB_ResourceDepot::ASAS_SB_ResourceDepot()
{
	Inventory = CreateDefaultSubobject<USAS_InventoryComponent>(TEXT("Inventory"));
		
}

USAS_InventoryComponent* ASAS_SB_ResourceDepot::GetInputInventory()
{
	return Inventory;
}


