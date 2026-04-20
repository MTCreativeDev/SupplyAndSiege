


#include "Core/Components/SAS_WorkerControlComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Core/SAS_GameplayTagContainer.h"
#include "Components/StateTreeComponent.h"
#include "AIController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"

USAS_WorkerControlComponent::USAS_WorkerControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

ESAS_WorkerRequestResult USAS_WorkerControlComponent::RequestManualMove(const FVector& WorldLocation)
{
	if (!UnitInformation) return ESAS_WorkerRequestResult::Rejected;
	if (CurrentWorkerControlState == ESAS_WorkerControlState::Transition) return ESAS_WorkerRequestResult::Rejected;
	if (!ExitLmControlForPlayerOverride()) return ESAS_WorkerRequestResult::Rejected;

	BeginManualMove_Internal(WorldLocation);
	return ESAS_WorkerRequestResult::Accepted;

}

ESAS_WorkerRequestResult USAS_WorkerControlComponent::RequestManualHarvest(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	if (!UnitInformation || !ResourceType) return ESAS_WorkerRequestResult::Rejected;
	if (CurrentWorkerControlState == ESAS_WorkerControlState::Transition) return ESAS_WorkerRequestResult::Rejected;
	if (!ExitLmControlForPlayerOverride()) return ESAS_WorkerRequestResult::Rejected;

	BeginManualHarvest_Internal(ResourceType, ResourceKey, ResourceLocation);
	return ESAS_WorkerRequestResult::Accepted;
}

ESAS_WorkerRequestResult USAS_WorkerControlComponent::RequestEnterLmQueue()
{
	if (CurrentWorkerControlState == ESAS_WorkerControlState::Transition) return ESAS_WorkerRequestResult::Rejected;
	if (CurrentWorkerControlState == ESAS_WorkerControlState::LMQueue) return ESAS_WorkerRequestResult::Accepted;

	if (CanEnterLmQueueImmediately())
	{
		FinishEnterLmQueue();
		return ESAS_WorkerRequestResult::Accepted;
	}

	StartTransitionIntoLmQueue();
	return ESAS_WorkerRequestResult::StartedTransition;
}

bool USAS_WorkerControlComponent::TryAcceptAssignment(USAS_LogisticsWorkerAssignment* NewAssignment)
{
	if (!NewAssignment) return false;
	if (CurrentWorkerControlState != ESAS_WorkerControlState::LMQueue) return false;
	if (ActiveAssignment) return false;

	ActiveAssignment = NewAssignment;
	return true;
}

void USAS_WorkerControlComponent::NotifyAssignmentFinished(USAS_LogisticsWorkerAssignment* FinishedAssignment)
{
	if (ActiveAssignment == FinishedAssignment)
	{
		ActiveAssignment = nullptr;
		//TODO:Review this to make sure nothing else is needed
	}
}

void USAS_WorkerControlComponent::NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* CancelledAssignment)
{
	if (ActiveAssignment == CancelledAssignment)
	{
		ActiveAssignment = nullptr;
		//TODO:Review this to make sure nothing else is needed
	}
}


bool USAS_WorkerControlComponent::UpdateCurrentHarvestKeyAndLocation(FSAS_ResourceKey ResourceKey, FVector TargetLocation)
{
	if (CurrentWorkerControlState != ESAS_WorkerControlState::ManualPersistent) return false;
	if (!CurrentHarvestResourceType) return false;

	CurrentHarvestResourceKey = ResourceKey;
	CurrentHarvestTargetLocation = TargetLocation;

	return true;
}

void USAS_WorkerControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		UnitInformation = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
	}

	if (UWorld* World = GetWorld())
	{
		AGameStateBase* GS = World->GetGameState();
		if (GS)
		{
			LogisticsManager = GS->FindComponentByClass<USAS_LogisticsManagerComponent>();
		}
	}
	
}

bool USAS_WorkerControlComponent::ExitLmControlForPlayerOverride()
{
	if (CurrentWorkerControlState != ESAS_WorkerControlState::LMQueue) return true;
	if (ActiveAssignment)
	{
		ActiveAssignment->CancelAssignment();
		ActiveAssignment = nullptr;
	}
	if (LogisticsManager)
	{
		LogisticsManager->UnregisterAvailableWorker(this);
	}

	CurrentWorkerControlState = ESAS_WorkerControlState::Idle;
	return true;
}

bool USAS_WorkerControlComponent::CanEnterLmQueueImmediately() const
{
	return true;
}

void USAS_WorkerControlComponent::StartTransitionIntoLmQueue()
{
	CurrentWorkerControlState = ESAS_WorkerControlState::Transition;
}

void USAS_WorkerControlComponent::FinishEnterLmQueue()
{
	if (LogisticsManager)
	{
		LogisticsManager->RegisterAvailableWorker(this);
	}

	CurrentWorkerControlState = ESAS_WorkerControlState::LMQueue;
}

void USAS_WorkerControlComponent::BeginManualMove_Internal(const FVector& WorldLocation)
{
	CurrentWorkerControlState = ESAS_WorkerControlState::ManualSingle;

}

void USAS_WorkerControlComponent::BeginManualHarvest_Internal(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	CurrentWorkerControlState = ESAS_WorkerControlState::ManualPersistent;
	CurrentHarvestResourceType = ResourceType;
	CurrentHarvestResourceKey = ResourceKey;
	CurrentHarvestTargetLocation = ResourceLocation;

	SendStateTreeEvent(SASGameplayTags::StateTree_Worker_HarvestOrder);
}

void USAS_WorkerControlComponent::SendStateTreeEvent(const FGameplayTag& EventTag)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	AAIController* AI = Cast<AAIController>(Pawn->GetController());
	if (!AI) return;

	UStateTreeComponent* ST = AI->FindComponentByClass<UStateTreeComponent>();
	if (!ST) return;

	ST->SendStateTreeEvent(FStateTreeEvent(EventTag));
}
