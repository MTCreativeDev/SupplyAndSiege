

#include "Core/Actors/SAS_FloatingIsland.h"
#include "Core/Components/SAS_IslandComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/CustomCollision.h"
#include "Engine/StaticMesh.h"
#include "Misc/DataAssets/SAS_IslandDefinitionData.h"

ASAS_FloatingIsland::ASAS_FloatingIsland()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	IslandAreaCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	IslandAreaCollision->SetCollisionProfileName(SAS_CollisionProfiles::IslandBounds);
	IslandAreaCollision->SetSimulatePhysics(false);
	IslandAreaCollision->SetupAttachment(SceneRoot);
	// TODO - Update FVector values with real numbers 
	IslandAreaCollision->SetBoxExtent(FVector(0.f, 0.f, 0.f));
	IslandAreaCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	IslandAreaCollision->SetGenerateOverlapEvents(true);
	IslandAreaCollision->SetHiddenInGame(false);

	IslandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IslandMesh"));
	IslandMesh->SetupAttachment(IslandAreaCollision);

	IslandLogic = CreateDefaultSubobject<USAS_IslandComponent>(TEXT("IslandLogic"));
}

void ASAS_FloatingIsland::BeginPlay()
{
	Super::BeginPlay();
	ApplyDefinitionToComponents();
}

void ASAS_FloatingIsland::ApplyDefinitionToComponents()
{
	if (!IslandDefinition) return;
	if (!IslandDefinition->IslandMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Island %s has no mesh!"), *GetName());
		return;
	}
	if (!IslandAreaCollision || !IslandMesh) return;

	IslandAreaCollision->SetBoxExtent(IslandDefinition->IslandAreaCollisionExtents);
	IslandAreaCollision->SetRelativeLocation(IslandDefinition->IslandAreaCollisionLocation);

	IslandMesh->SetStaticMesh(IslandDefinition->IslandMesh);
	IslandMesh->SetRelativeTransform(IslandDefinition->IslandMeshRelativeTransform);

	IslandLogic->CaeliumRemaining = IslandDefinition->CaeliumDeposits;
	UE_LOG(LogTemp, Display, TEXT("Island display name %s - %s"), *GetName(), *IslandDefinition->DisplayName.ToString());
}

void ASAS_FloatingIsland::SetIslandDefinition(USAS_IslandDefinitionData* NewIslandDefinition)
{
	if (!NewIslandDefinition) return;
	IslandDefinition = NewIslandDefinition;
}