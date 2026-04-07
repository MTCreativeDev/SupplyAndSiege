


#include "Core/Components/SAS_IslandManagerComponent.h"
#include "Core/Components/SAS_IslandComponent.h"

USAS_IslandManagerComponent::USAS_IslandManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_IslandManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USAS_IslandManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 USAS_IslandManagerComponent::GetHealth(USAS_IslandComponent* IslandComponent) const
{
	return IslandComponent->GetHealth();
}

bool USAS_IslandManagerComponent::IsDepleted(const int32 islandHealth) const
{
	return islandHealth <= 0;
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

void USAS_IslandManagerComponent::CheckIslandHealth(USAS_IslandComponent* IslandComponent)
{
	if (!IslandComponent) return;
	int32 health = GetHealth(IslandComponent); 
	bool isDepleted = IsDepleted(health);
	if (isDepleted)
	{
		RemoveIslandFromQueue(IslandComponent);
	}
}

void USAS_IslandManagerComponent::RemoveIslandFromQueue(USAS_IslandComponent* IslandComponent)
{
	if (!IslandComponent) return;
	IslandQueue.Remove(TWeakObjectPtr<USAS_IslandComponent>(IslandComponent));
}

void USAS_IslandManagerComponent::CleanupIslandQueue()
{
	IslandQueue.RemoveAll([](const TWeakObjectPtr<USAS_IslandComponent>& Ptr)
		{
			return !Ptr.IsValid();
		});
}