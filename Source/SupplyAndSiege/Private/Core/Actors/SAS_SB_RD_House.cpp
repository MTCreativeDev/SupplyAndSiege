


#include "Core/Actors/SAS_SB_RD_House.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_PopulationManagerComponent.h"

ASAS_SB_RD_House::ASAS_SB_RD_House()
{

}

void ASAS_SB_RD_House::BeginPlay()
{
	GetPopulationManager();

	if (!PopulationManagerComponent) return;
	PopulationManagerComponent->RegisterHousing(this);
	
}

void ASAS_SB_RD_House::GetPopulationManager()
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!IsValid(GS)) return;

	PopulationManagerComponent = GS->FindComponentByClass<USAS_PopulationManagerComponent>();
}
