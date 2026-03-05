


#include "Core/Actors/SAS_SB_ResourceDepot.h"
#include "Core/Components/SAS_InventoryComponent.h"



ASAS_SB_ResourceDepot::ASAS_SB_ResourceDepot()
{
	Inventory = CreateDefaultSubobject<USAS_InventoryComponent>(TEXT("Inventory"));

	DropOffLocationContainer = CreateDefaultSubobject<USceneComponent>(TEXT("DropOffLocationContainer"));
	DropOffLocationContainer->SetupAttachment(SceneRoot);
}

const TArray<FVector> ASAS_SB_ResourceDepot::GetDropOffLocations() const
{
	return DropOffLocations_World;
}

void ASAS_SB_ResourceDepot::BeginPlay()
{
	Super::BeginPlay();

	RebuildDropoffCache();
}

void ASAS_SB_ResourceDepot::RebuildDropoffCache()
{
	DropOffLocations_World.Reset();

	if (!DropOffLocationContainer) return;

	TArray<USceneComponent*> DropoffChildren;
	DropOffLocationContainer->GetChildrenComponents(true, DropoffChildren);

	for (USceneComponent* Child : DropoffChildren)
	{
		if (!Child) continue;
		DropOffLocations_World.Add(Child->GetComponentLocation());
	}
}
