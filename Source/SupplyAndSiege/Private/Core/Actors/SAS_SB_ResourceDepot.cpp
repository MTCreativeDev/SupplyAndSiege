


#include "Core/Actors/SAS_SB_ResourceDepot.h"
#include "Core/Components/Inventory/SAS_IC_ResourceDepot.h"
#include "Components/StaticMeshComponent.h"
#include "Core/CustomCollision.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_WinLossManagerComponent.h"



ASAS_SB_ResourceDepot::ASAS_SB_ResourceDepot()
{
	Inventory = CreateDefaultSubobject<USAS_IC_ResourceDepot>(TEXT("Inventory"));
	PrimaryMesh->SetCollisionProfileName(SAS_CollisionProfiles::BuildingMesh);

		
}

USAS_InventoryComponent* ASAS_SB_ResourceDepot::GetInputInventory()
{
	return Inventory;
}

void ASAS_SB_ResourceDepot::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!IsValid(GS)) return;

	USAS_WinLossManagerComponent* WinLossComponent = GS->FindComponentByClass< USAS_WinLossManagerComponent>();

	if (!IsValid(WinLossComponent)) return;

	WinLossComponent->NotifyBuildingCompleted(this);

}


