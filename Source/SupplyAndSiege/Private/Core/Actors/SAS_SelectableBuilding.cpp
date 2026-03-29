
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/DataAssets/SAS_BuildingDefinitionData.h"

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

	if (!BuildingDefinition) return;
	MoveToLocations_World.Reserve(BuildingDefinition->MoveToRelativeLocations.Num());

	const FTransform ActorTransform = GetActorTransform();

	for (const FVector& LocalLocation : BuildingDefinition->MoveToRelativeLocations)
	{
		MoveToLocations_World.Add(ActorTransform.TransformPosition(LocalLocation));
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


