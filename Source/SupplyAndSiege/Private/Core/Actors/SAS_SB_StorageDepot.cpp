


#include "Core/Actors/SAS_SB_StorageDepot.h"
#include "Core/Components/Inventory/SAS_IC_ResourceDepot.h"

ASAS_SB_StorageDepot::ASAS_SB_StorageDepot()
{
	Inventory = CreateDefaultSubobject<USAS_IC_ResourceDepot>(TEXT("Inventory"));
}
