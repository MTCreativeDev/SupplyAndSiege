
#include "Core/Components/SAS_IslandManagerComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Core/Components/SAS_IslandComponent.h"

USAS_IslandManagerComponent::USAS_IslandManagerComponent()
{
}

void USAS_IslandManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	// periodically check island caelium deposits
    GetWorld()->GetTimerManager().SetTimer(
        IslandCheckTimerHandle,
        this,
        &USAS_IslandManagerComponent::UpdateIslandStatuses,
        60.0f,  // Check every 60 second
        true   
    );
}

int32 USAS_IslandManagerComponent::GetCaeliumDeposits(USAS_IslandComponent* IslandComponent) const
{
	int32 CaeliumDeposits = IslandComponent->GetCaeliumDeposits();
	UE_LOG(LogTemp, Display, TEXT("Island Caelium Deposits: %d"), CaeliumDeposits);
	return CaeliumDeposits;
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
    if (AActor* IslandActor = IslandComponent->GetOwner())
    {
        UE_LOG(LogTemp, Display, TEXT("Island added to queue: %s"), *IslandActor->GetName());
    }
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
	UE_LOG(LogTemp, Display, TEXT("Islands: %d"), ValidIslands.Num());
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
	if (AActor* IslandActor = IslandComponent->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("IslandCaeliumDeposits: %s, %d"), *IslandActor->GetName(), Caelium);
	}
}

void USAS_IslandManagerComponent::UpdateIslandStatuses()
{
    TArray<USAS_IslandComponent*> Islands = GetIslandComponents();
    for (USAS_IslandComponent* Island : Islands)
    {
        CheckIslandCaeliumDeposits(Island);
    }
}

void USAS_IslandManagerComponent::RemoveIslandFromQueue(USAS_IslandComponent* IslandComponent)
{
	if (!IslandComponent) return;
	IslandQueue.Remove(TWeakObjectPtr<USAS_IslandComponent>(IslandComponent));
	if (AActor* IslandActor = IslandComponent->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove Island from Queue: %s"), *IslandActor->GetName());
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