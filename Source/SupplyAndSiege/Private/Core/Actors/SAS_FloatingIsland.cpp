


#include "Core/Actors/SAS_FloatingIsland.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ASAS_FloatingIsland::ASAS_FloatingIsland()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASAS_FloatingIsland::BeginPlay()
{
	Super::BeginPlay();
}

void ASAS_FloatingIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


