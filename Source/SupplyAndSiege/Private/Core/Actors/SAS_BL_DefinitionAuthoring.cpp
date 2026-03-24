


#include "Core/Actors/SAS_BL_DefinitionAuthoring.h"
#include "Misc/DataAssets/SAS_BuildingDefinitionData.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

#if WITH_EDITOR
#include "UObject/Package.h"
#endif

ASAS_BL_DefinitionAuthoring::ASAS_BL_DefinitionAuthoring()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASAS_BL_DefinitionAuthoring::LoadFromDefinition()
{
	if (!BuildingDefinition) return;
	if(!BuildingAreaCollision || !PrimaryMesh || !SecondaryMesh) return;

	BuildingAreaCollision->SetBoxExtent(BuildingDefinition->BuildingAreaCollisionExtents);
	BuildingAreaCollision->SetRelativeLocation(BuildingDefinition->BuildingAreaCollisionLocation);

	PrimaryMesh->SetStaticMesh(BuildingDefinition->PrimaryMesh);
	PrimaryMesh->SetRelativeTransform(BuildingDefinition->PrimaryMeshRelativeTransform);

	SecondaryMesh->SetStaticMesh(BuildingDefinition->SecondaryMesh);
	SecondaryMesh->SetRelativeTransform(BuildingDefinition->SecondaryMeshRelativeTransform);
}

void ASAS_BL_DefinitionAuthoring::ApplyToDefinition()
{
#if WITH_EDITOR
	if (!BuildingDefinition) return;
	if (!BuildingAreaCollision || !PrimaryMesh || !SecondaryMesh) return;

	BuildingDefinition->Modify();

	BuildingDefinition->BuildingAreaCollisionExtents = BuildingAreaCollision->GetUnscaledBoxExtent();
	BuildingDefinition->BuildingAreaCollisionLocation = BuildingAreaCollision->GetRelativeLocation();

	BuildingDefinition->PrimaryMesh = PrimaryMesh->GetStaticMesh();
	BuildingDefinition->PrimaryMeshRelativeTransform = PrimaryMesh->GetRelativeTransform();

	BuildingDefinition->SecondaryMesh = SecondaryMesh->GetStaticMesh();
	BuildingDefinition->SecondaryMeshRelativeTransform = SecondaryMesh->GetRelativeTransform();

	CaptureMoveToLocationsToDefinition();

	BuildingDefinition->MarkPackageDirty();
#endif
}

void ASAS_BL_DefinitionAuthoring::CaptureMoveToLocationsToDefinition()
{
#if WITH_EDITOR
	if (!BuildingDefinition || !MoveToLocationsContainer) return;

	BuildingDefinition->MoveToRelativeLocations.Empty();

	TArray<USceneComponent*> MoveToChildren;
	MoveToLocationsContainer->GetChildrenComponents(false, MoveToChildren);

	for (USceneComponent* Child : MoveToChildren)
	{
		if (!Child) continue;
		BuildingDefinition->MoveToRelativeLocations.Add(Child->GetRelativeLocation());
	}

#endif
}
