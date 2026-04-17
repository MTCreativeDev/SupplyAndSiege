


#include "Core/Components/SAS_WorkerControlComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "GameFramework/GameStateBase.h"

USAS_WorkerControlComponent::USAS_WorkerControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

ESAS_WorkerRequestResult USAS_WorkerControlComponent::RequestManualMove(const FVector& WorldLocation)
{
	if (!UnitInformation) return ESAS_WorkerRequestResult::Rejected;
	if (CurrentControlState == ESAS_WorkerControlState::Transition) return ESAS_WorkerRequestResult::Rejected;
	if (!ExitLmControlForPlayerOverride()) return ESAS_WorkerRequestResult::Rejected;

	BeginManualMove_Internal(WorldLocation);
	return ESAS_WorkerRequestResult::Accepted;

}

ESAS_WorkerRequestResult USAS_WorkerControlComponent::RequestManualHarvest(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	if (!UnitInformation || !ResourceType) return ESAS_WorkerRequestResult::Rejected;
	if (CurrentControlState == ESAS_WorkerControlState::Transition) return ESAS_WorkerRequestResult::Rejected;
	if (!ExitLmControlForPlayerOverride()) return ESAS_WorkerRequestResult::Rejected;

	BeginManualHarvest_Internal(ResourceType, ResourceKey, ResourceLocation);
	return ESAS_WorkerRequestResult::Accepted;
}

ESAS_WorkerRequestResult USAS_WorkerControlComponent::RequestEnterLmQueue()
{
	if (CurrentControlState == ESAS_WorkerControlState::Transition) return ESAS_WorkerRequestResult::Rejected;
	if (CurrentControlState == ESAS_WorkerControlState::LMQueue) return ESAS_WorkerRequestResult::Accepted;

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
	if (CurrentControlState != ESAS_WorkerControlState::LMQueue) return false;
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
	if (CurrentControlState != ESAS_WorkerControlState::LMQueue) return true;
	if (ActiveAssignment)
	{
		ActiveAssignment->CancelAssignment();
		ActiveAssignment = nullptr;
	}
	if (LogisticsManager)
	{
		LogisticsManager->UnregisterAvailableWorker(this);
			//TODO: Need to create this functionality in the LM
	}

	CurrentControlState = ESAS_WorkerControlState::Idle;
	return true;
}

bool USAS_WorkerControlComponent::CanEnterLmQueueImmediately() const
{
	return true;
	//This will be updated in the child components
}

void USAS_WorkerControlComponent::StartTransitionIntoLmQueue()
{
	CurrentControlState = ESAS_WorkerControlState::Transition;
}

void USAS_WorkerControlComponent::FinishEnterLmQueue()
{
	if (LogisticsManager)
	{
		LogisticsManager->RegisterAvailableWorker(this);
		//TODO Need to implement this function
	}

	CurrentControlState = ESAS_WorkerControlState::LMQueue;
}

void USAS_WorkerControlComponent::BeginManualMove_Internal(const FVector& WorldLocation)
{
	CurrentControlState = ESAS_WorkerControlState::ManualSingle;
	UnitInformation->IssueMoveOrder(WorldLocation);
	//TODO This should be handled via the state tree I think. Will likely remove any functions like the move order out of the UIC. This should likely be in the UnitControlComponent since it isnt exclusive to villagers/couriers.
}

void USAS_WorkerControlComponent::BeginManualHarvest_Internal(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	CurrentControlState = ESAS_WorkerControlState::ManualPersistent;
	UnitInformation->IssueHarvestOrder(ResourceType, ResourceKey, ResourceLocation);
	//TODO this should be handled in the state tree as well.
}


