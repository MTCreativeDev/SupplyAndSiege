
#include "Core/Components/SAS_IslandManagerComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
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
	if (IslandsBeingDestroyed.Contains(TWeakObjectPtr<USAS_IslandComponent>(IslandComponent)))
	{
		return;
	}

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
	AActor* IslandActor = IslandComponent->GetOwner();
	if (!IslandActor) return;
	IslandsBeingDestroyed.Add(TWeakObjectPtr<USAS_IslandComponent>(IslandComponent));

	UE_LOG(LogTemp, Warning, TEXT("Initiating island destruction: %s"), *IslandActor->GetName());
	IslandComponent->PlayFallAnimation();
	OnIslandDestroyed.Broadcast(IslandComponent->GetIslandID(), IslandActor);
	const float DestructionDelay2Seconds = 2.0f;
	FTimerHandle DestructionTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestructionTimerHandle,
		[IslandActor]()
		{
			if (IslandActor && !IslandActor->IsActorBeingDestroyed())
			{
				IslandActor->Destroy();
			}
		},
		DestructionDelay2Seconds,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Island %s scheduled for destruction in %.1f seconds"),
		*IslandActor->GetName(), DestructionDelay2Seconds);
}

void USAS_IslandManagerComponent::CleanupIslandQueue()
{
	IslandQueue.RemoveAll([](const TWeakObjectPtr<USAS_IslandComponent>& Ptr)
		{
			return !Ptr.IsValid();
		});
}

void USAS_IslandManagerComponent::TestIslandDestruction()
{
    UE_LOG(LogTemp, Warning, TEXT("=== Test: Island Destruction Start ==="));
    TArray<USAS_IslandComponent*> Islands = GetIslandComponents();

    if (Islands.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No existing islands found to test destruction on. Spawn some islands first!"));
        return;
    }

    USAS_IslandComponent* TargetIsland = Islands[0];
    if (!TargetIsland)
    {
        UE_LOG(LogTemp, Error, TEXT("Target Island pointer is null!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Found %d island(s). Testing on: %s"), Islands.Num(), *TargetIsland->GetName());

    const int32 TestCaeliumAmount = 100;
	TargetIsland->SetCaeliumDeposits(TestCaeliumAmount);
	int32 InitialCaelium = TargetIsland->GetCaeliumDeposits();
	UE_LOG(LogTemp, Log, TEXT("Target Island prepared with initial caelium: %d"), InitialCaelium);
    UE_LOG(LogTemp, Log, TEXT("Depleting all caelium (%d)..."), InitialCaelium);
    TargetIsland->SubtractCaelium(InitialCaelium);

    int32 DepletedCaelium = TargetIsland->GetCaeliumDeposits();
    if (DepletedCaelium == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Caelium successfully depleted to 0."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Caelium is: %d (expected 0)"), DepletedCaelium);
        return;
    }
    
	CheckIslandCaeliumDeposits(TargetIsland);

	if (IslandsBeingDestroyed.Contains(TargetIsland))
    {
        UE_LOG(LogTemp, Log, TEXT("Island successfully marked for destruction"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Island was NOT marked for destruction"));
    }

    TArray<USAS_IslandComponent*> RemainingIslands = GetIslandComponents();
    if (!RemainingIslands.Contains(TargetIsland))
    {
        UE_LOG(LogTemp, Log, TEXT("Island removed from active queue"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Island is still in the active queue!"));
    }

    UE_LOG(LogTemp, Warning, TEXT("=== Test: Island Destruction Complete ==="));
}

static FAutoConsoleCommand TestIslandDestructionCmd(
	TEXT("Island.TestDestruction"),
	TEXT("Test the island destruction system"),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (!World) return;

			for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
			{
				if (USAS_IslandManagerComponent* Manager = (*ActorIt)->FindComponentByClass<USAS_IslandManagerComponent>())
				{
					Manager->TestIslandDestruction();
					return;
				}
			}
			UE_LOG(LogTemp, Error, TEXT("No IslandManagerComponent found in world!"));
		})
);