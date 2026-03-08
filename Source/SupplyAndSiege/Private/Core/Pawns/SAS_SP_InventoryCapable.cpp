


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
	InventoryCheckCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASAS_SP_InventoryCapable::BeginLookingForInventory(USAS_InventoryComponent* InTargetInventory)
{
	if (!InTargetInventory)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginLookingForInventory failed: InTargetInventory was null"));
		return;
	}

	AActor* InventoryOwner = InTargetInventory->GetOwner();
	if (!InventoryOwner)
	{
				if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginLookingForInventory failed: Target inventory has no owner"));
		return;
	}

	TargetInventory = InTargetInventory;
	TargetInventoryOwner = InventoryOwner;
	bLookingForInventory = true;

	if (!InventoryCheckCollision)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginLookingForInventory partially failed: InventoryCheckCollision is missing"));
		return;
	}

	InventoryCheckCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	

}

void ASAS_SP_InventoryCapable::StopLookingForInventory()
{
	TargetInventory = nullptr;
	TargetInventoryOwner = nullptr;
	bLookingForInventory = false;

	if (!InventoryCheckCollision)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("StopLookingForInventory partially failed: InventoryCheckCollision is missing"));
		return;
	}

	InventoryCheckCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

USAS_InventoryComponent* ASAS_SP_InventoryCapable::GetInventory() const
{
	return Inventory;
}
