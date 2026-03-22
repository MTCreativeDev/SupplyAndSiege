


#include "Core/Actors/SAS_BuildingLayout.h"
#include "Components/BoxComponent.h"
#include "Core/CustomCollision.h"
#include "Misc/DataAssets/SAS_BuildingDefinitionData.h"

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
	BuildingAreaCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BuildingAreaCollision->SetGenerateOverlapEvents(true);
	BuildingAreaCollision->SetHiddenInGame(false);


	PrimaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimaryMesh"));
	PrimaryMesh->SetupAttachment(BuildingAreaCollision);

	SecondaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryMesh"));
	SecondaryMesh->SetupAttachment(BuildingAreaCollision);

	MoveToLocationsContainer = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToLocationsContainer"));
	MoveToLocationsContainer->SetupAttachment(SceneRoot);
	
}

#if WITH_EDITOR
void ASAS_BuildingLayout::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ASAS_BuildingLayout, BuildingDefinition))
	{
		ApplyDefinitionToComponents();
	}
}
#endif


void ASAS_BuildingLayout::ApplyDefinitionToComponents()
{
	if (!BuildingDefinition) return;
	if (!BuildingAreaCollision || !PrimaryMesh || !SecondaryMesh) return;

	BuildingAreaCollision->SetBoxExtent(BuildingDefinition->BuildingAreaCollisionExtents);
	BuildingAreaCollision->SetRelativeLocation(BuildingDefinition->BuildingAreaCollisionLocation);

	PrimaryMesh->SetStaticMesh(BuildingDefinition->PrimaryMesh);
	PrimaryMesh->SetRelativeTransform(BuildingDefinition->PrimaryMeshRelativeTransform);

	SecondaryMesh->SetStaticMesh(BuildingDefinition->SecondaryMesh);
	SecondaryMesh->SetRelativeTransform(BuildingDefinition->SecondaryMeshRelativeTransform);

}

void ASAS_BuildingLayout::SetBuildingDefinition(USAS_BuildingDefinitionData* NewBuildingDefinition)
{
	if (!NewBuildingDefinition) return;
	
	BuildingDefinition = NewBuildingDefinition;
}


