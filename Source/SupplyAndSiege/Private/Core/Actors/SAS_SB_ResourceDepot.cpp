


#include "Core/Actors/SAS_SB_ResourceDepot.h"
#include "Core/Components/Inventory/SAS_IC_ResourceDepot.h"
#include "Components/StaticMeshComponent.h"
#include "Core/CustomCollision.h"



ASAS_SB_ResourceDepot::ASAS_SB_ResourceDepot()
{
	Inventory = CreateDefaultSubobject<USAS_IC_ResourceDepot>(TEXT("Inventory"));
	PrimaryMesh->SetCollisionProfileName(SAS_CollisionProfiles::BuildingMesh);

		
}

USAS_InventoryComponent* ASAS_SB_ResourceDepot::GetInputInventory()
{
	return Inventory;
}


