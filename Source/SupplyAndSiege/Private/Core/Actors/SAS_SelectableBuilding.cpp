
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ASAS_SelectableBuilding::ASAS_SelectableBuilding()
{
	PrimaryActorTick.bCanEverTick = false;

	UnitInformationComponent = CreateDefaultSubobject<USAS_UnitInformationComponent>(TEXT("UnitInformationComponent"));
}


void ASAS_SelectableBuilding::BeginPlay()
{
	Super::BeginPlay();

	UnitInformationComponent->SetTeam(AssignTeamOnSpawn);
	RebuildMoveToLocations();
	
}

void ASAS_SelectableBuilding::RebuildMoveToLocations()
{
	MoveToLocations_World.Reset();

	if (!MoveToLocationsContainer) return;

	TArray<USceneComponent*> MoveToChildren;
	MoveToLocationsContainer->GetChildrenComponents(true, MoveToChildren);

	for (USceneComponent* Child : MoveToChildren)
	{
		if (!Child) continue;
		MoveToLocations_World.Add(Child->GetComponentLocation());
	}
}

void ASAS_SelectableBuilding::DestroySelf()
{
	UnitInformationComponent->RemoveUnitFromGame();
	Destroy();
}

const TArray<FVector> ASAS_SelectableBuilding::GetMoveToLocations() const
{
	return MoveToLocations_World;
}


