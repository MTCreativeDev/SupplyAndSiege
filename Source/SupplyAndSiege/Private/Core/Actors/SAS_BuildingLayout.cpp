


#include "Core/Actors/SAS_BuildingLayout.h"
#include "Components/BoxComponent.h"
#include "Core/CustomCollision.h"

ASAS_BuildingLayout::ASAS_BuildingLayout()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BuildingAreaCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BuildingAreaCollision->SetCollisionProfileName(SAS_CollisionProfiles::BuildingBounds);
	BuildingAreaCollision->SetSimulatePhysics(false);
	BuildingAreaCollision->SetupAttachment(SceneRoot);
	BuildingAreaCollision->SetBoxExtent(FVector(800.f, 800.f, 400.f));

	PrimaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimaryMesh"));
	PrimaryMesh->SetupAttachment(BuildingAreaCollision);

	SecondaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryMesh"));
	SecondaryMesh->SetupAttachment(BuildingAreaCollision);

	MoveToLocationsContainer = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToLocationsContainer"));
	MoveToLocationsContainer->SetupAttachment(SceneRoot);
	
}


