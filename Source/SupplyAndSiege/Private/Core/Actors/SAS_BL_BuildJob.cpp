


#include "Core/Actors/SAS_BL_BuildJob.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Misc/DataAssets/SAS_BuildingDefinitionData.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"

ASAS_BL_BuildJob::ASAS_BL_BuildJob()
{
	UnitInformation = CreateDefaultSubobject<USAS_UnitInformationComponent>(TEXT("UnitInformation"));
	Inventory = CreateDefaultSubobject< USAS_InventoryComponent>(TEXT("Inventory"));

}

void ASAS_BL_BuildJob::InitializeBuildJob(ESAS_Team NewAssignedTeam)
{
	UnitInformation->SetTeam(NewAssignedTeam);

	if (!ConstructionInventoryProfile) return;
	Inventory->SetInventoryProfile(ConstructionInventoryProfile);

	RequestResourceDeliveryJobs();

	return;
}

void ASAS_BL_BuildJob::RequestResourceDeliveryJobs()
{
	if (!BuildingDefinition) return;
	if (BuildingDefinition->BuildingCost.ResourceCost.IsEmpty()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!GS) return;

	USAS_LogisticsManagerComponent* LMC = GS->FindComponentByClass<USAS_LogisticsManagerComponent>();
	if (!LMC) return;

	FSAS_ResourceDeliveryRequest DeliveryRequest;

	DeliveryRequest.BuildSite = this;
	DeliveryRequest.ResourceCost = BuildingDefinition->BuildingCost.ResourceCost;
	DeliveryRequest.Priority = 1;
		//TODO: Implement priority

	LMC->CreateBuildSiteResourceDeliveryJob(DeliveryRequest);
}
