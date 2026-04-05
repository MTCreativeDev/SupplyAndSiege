


#include "Core/Components/SAS_IslandComponent.h"

USAS_IslandComponent::USAS_IslandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	IslandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IslandMesh"));
}

void USAS_IslandComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IslandID.IsValid())
	{
		IslandID = FGuid::NewGuid();
	}
}

void USAS_IslandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FGuid USAS_IslandComponent::GetIslandID()
{
	if (IslandID.IsValid())
	{
		return IslandID;
	}

	return FGuid();
}

