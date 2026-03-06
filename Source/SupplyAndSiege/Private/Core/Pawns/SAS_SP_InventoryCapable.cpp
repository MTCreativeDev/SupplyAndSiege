


#include "Core/Pawns/SAS_SP_InventoryCapable.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Components/BoxComponent.h"
#include "Core/CustomCollision.h"

ASAS_SP_InventoryCapable::ASAS_SP_InventoryCapable()
{
	Inventory = CreateDefaultSubobject<USAS_InventoryComponent>(TEXT("Inventory"));

	InventoryCheckCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InventoryCheckCollision"));
	InventoryCheckCollision->SetupAttachment(GetRootComponent());
	InventoryCheckCollision->SetCollisionProfileName(SAS_CollisionProfiles::InventoryCheck);
}
