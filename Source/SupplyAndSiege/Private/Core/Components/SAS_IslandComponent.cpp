


#include "Core/Components/SAS_IslandComponent.h"

USAS_IslandComponent::USAS_IslandComponent()
{
}

void USAS_IslandComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IslandID.IsValid())
	{
		IslandID = FGuid::NewGuid();
	}
}

// TODO: Add logic to get health from the island component instead of returning a placeholder value.
int32 USAS_IslandComponent::GetHealth() const
{
	return Health;
}

FGuid USAS_IslandComponent::GetIslandID() const
{
	if (IslandID.IsValid())
	{
		return IslandID;
	}

	return FGuid();
}

