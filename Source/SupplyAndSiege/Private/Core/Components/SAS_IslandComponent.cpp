


#include "Core/Components/SAS_IslandComponent.h"
#include "Core/Components/SAS_IslandManagerComponent.h" 
#include "EngineUtils.h" 

class USAS_IslandManagerComponent;

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

	if (AActor* OwnerActor = GetOwner())
	{
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			if (USAS_IslandManagerComponent* Manager = (*It)->FindComponentByClass<USAS_IslandManagerComponent>())
			{
				Manager->AddIslandToQueue(this);
				break;
			}
		}
	}
}

int32 USAS_IslandComponent::GetCaeliumDeposits() const
{
	return CaeliumRemaining;
}

FGuid USAS_IslandComponent::GetIslandID() const
{
	if (IslandID.IsValid())
	{
		return IslandID;
	}

	return FGuid();
}

int32 USAS_IslandComponent::MineCaeliumDeposits()
{

	int32 AmountMined = FMath::Min(CaeliumMined, CaeliumRemaining);
	CaeliumRemaining -= AmountMined;
	return AmountMined;
}

void USAS_IslandComponent::PlayFallAnimation()
{
	// TODO -  Trigger fall anim, disable collision
}