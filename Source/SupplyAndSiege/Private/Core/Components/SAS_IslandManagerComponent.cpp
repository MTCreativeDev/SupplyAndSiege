


#include "Core/Components/SAS_IslandManagerComponent.h"
#include "Core/Components/SAS_IslandComponent.h"

USAS_IslandManagerComponent::USAS_IslandManagerComponent()
{
}

void USAS_IslandManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

int32 USAS_IslandManagerComponent::GetCaeliumDeposits(USAS_IslandComponent* IslandComponent) const
{
	return IslandComponent->GetCaeliumDeposits();
}

bool USAS_IslandManagerComponent::IsDepleted(const int32 IslandCaelium) const
{
	return IslandCaelium <= 0;
}

bool USAS_IslandManagerComponent::CanIslandFloat(const int32 IslandCaelium) const
{
	return IslandCaelium >= MinCaeliumToFloat;
}

void USAS_IslandManagerComponent::AddIslandToQueue(USAS_IslandComponent* IslandComponent)
{
	if (!IslandComponent) return;
	IslandQueue.AddUnique(TWeakObjectPtr<USAS_IslandComponent>(IslandComponent));
}

TArray<USAS_IslandComponent*> USAS_IslandManagerComponent::GetIslandComponents() const
{
	TArray<USAS_IslandComponent*> ValidIslands;
	for (const TWeakObjectPtr<USAS_IslandComponent>& Ptr : IslandQueue)
	{
		if (Ptr.IsValid())
		{
			ValidIslands.Add(Ptr.Get());
		}
	}
	return ValidIslands;
}

void USAS_IslandManagerComponent::CheckIslandCaeliumDeposits(USAS_IslandComponent* IslandComponent)
{
	if (!IslandComponent) return;
	int32 Caelium = GetCaeliumDeposits(IslandComponent); 
	bool IsCaeliumDepleted = IsDepleted(Caelium);
	bool EnoughCaelium = CanIslandFloat(Caelium);
	if (IsCaeliumDepleted || !EnoughCaelium)
	{
		RemoveIslandFromQueue(IslandComponent);
	}
}

void USAS_IslandManagerComponent::RemoveIslandFromQueue(USAS_IslandComponent* IslandComponent)
{
	if (!IslandComponent) return;
	IslandQueue.Remove(TWeakObjectPtr<USAS_IslandComponent>(IslandComponent));
	if (AActor* IslandActor = IslandComponent->GetOwner())
	{
		IslandComponent->PlayFallAnimation();
		IslandActor->Destroy();
	}
}

void USAS_IslandManagerComponent::CleanupIslandQueue()
{
	IslandQueue.RemoveAll([](const TWeakObjectPtr<USAS_IslandComponent>& Ptr)
		{
			return !Ptr.IsValid();
		});
}