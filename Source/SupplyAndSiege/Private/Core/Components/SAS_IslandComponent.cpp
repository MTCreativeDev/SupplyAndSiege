


#include "Core/Components/SAS_IslandComponent.h"
#include "Core/Components/SAS_IslandManagerComponent.h" 
#include "Core/Actors/SAS_FloatingIsland.h"
#include "EngineUtils.h" 

class USAS_IslandManagerComponent;

USAS_IslandComponent::USAS_IslandComponent()
{
}

void USAS_IslandComponent::BeginPlay()
{
	Super::BeginPlay();
	IslandActor = GetOwner();

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
				IslandManager = Manager;
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
	UE_LOG(LogTemp, Display, TEXT("AmountMined: %s, %d"), *IslandActor->GetName(), AmountMined);
	CaeliumRemaining -= AmountMined;
	UE_LOG(LogTemp, Display, TEXT("CaeliumRemaining: %s, %d"), *IslandActor->GetName(), CaeliumRemaining);
	if (IslandManager && CaeliumRemaining <= IslandManager->MinCaeliumToFloat)
	{
		IslandManager->RemoveIslandFromQueue(this);
	}
	return AmountMined;
}

void USAS_IslandComponent::PlayFallAnimation()
{
	// TODO -  Trigger fall anim, disable collision
}


// FOR DEBUGGING - TODO: Remove when no longer needed for testing
static FAutoConsoleCommand GCmdMineCaelium(
    TEXT("Island.Mine"),
    TEXT("Mine caelium from the island"),
    FConsoleCommandDelegate::CreateLambda([]()
    {
        for (TActorIterator<ASAS_FloatingIsland> It(GWorld); It; ++It)
        {
            if (USAS_IslandComponent* IslandComp = It->FindComponentByClass<USAS_IslandComponent>())
            {
                IslandComp->MineCaeliumDeposits();
            }
        }
    }),
    ECVF_Cheat
);