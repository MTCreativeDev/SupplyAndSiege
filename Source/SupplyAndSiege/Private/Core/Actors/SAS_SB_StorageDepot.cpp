


#include "Core/Actors/SAS_SB_StorageDepot.h"
#include "Core/Components/SAS_InventoryComponent.h"

ASAS_SB_StorageDepot::ASAS_SB_StorageDepot()
{
	Inventory = CreateDefaultSubobject<USAS_InventoryComponent>(TEXT("Inventory"));
}
