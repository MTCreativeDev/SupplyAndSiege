


#include "Core/Actors/SAS_BL_BuildJob.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"

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

	return;
}
