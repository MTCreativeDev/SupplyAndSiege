


#include "Core/Actors/SAS_FloatingIsland.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

TArray<TWeakObjectPtr<ASAS_FloatingIsland>> ASAS_FloatingIsland::GUIDAssignmentQueue;

// Sets default values
ASAS_FloatingIsland::ASAS_FloatingIsland()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FloatingIslandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloatingIslandMesh"));
	RootComponent = FloatingIslandMesh;
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh)
	{
		FloatingIslandMesh->SetStaticMesh(CubeMesh);
	}
}

// Called when the game starts or when spawned
void ASAS_FloatingIsland::BeginPlay()
{
	Super::BeginPlay();
	if (!IslandID.IsValid())
	{
		IslandID = FGuid::NewGuid();
	}
	GUIDAssignmentQueue.Add(TWeakObjectPtr<ASAS_FloatingIsland>(this));
}

// Called every frame
void ASAS_FloatingIsland::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGuid ASAS_FloatingIsland::GetIslandID()
{
	if (IslandID.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Added Island %s to GUID assignment queue."), *GetName());
		return IslandID;
	}

	if (!GUIDAssignmentQueue.Contains(this))
	{
		GUIDAssignmentQueue.Add(TWeakObjectPtr<ASAS_FloatingIsland>(this));
		UE_LOG(LogTemp, Warning, TEXT("Island %s queued for GUID assignment."), *GetName());
	}

	return FGuid();
}

void ASAS_FloatingIsland::CleanupGUIDAssignmentQueue()
{
	GUIDAssignmentQueue.RemoveAll([](const TWeakObjectPtr<ASAS_FloatingIsland>& Ptr)
		{
			return !Ptr.IsValid();
		});
}