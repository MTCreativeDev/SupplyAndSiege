#include "Core/Components/SAS_WorkerControlComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
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
	if (!LogisticsManager) return ESAS_WorkerRequestResult::Rejected;
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
	CurrentWorkerControlState = ESAS_WorkerControlState::LMAssignment;
	return true;
}

void USAS_WorkerControlComponent::NotifyAssignmentEnded(USAS_LogisticsWorkerAssignment* EndedAssignment)
{
	if (ActiveAssignment != EndedAssignment) return;

	ActiveAssignment = nullptr;

	if (LogisticsManager)
	{
		LogisticsManager->UnregisterAvailableWorker(this);
	}

	CurrentWorkerControlState = ESAS_WorkerControlState::Idle;
	RequestEnterLmQueue();
}

bool USAS_WorkerControlComponent::UpdateCurrentHarvestKeyAndLocation(FSAS_ResourceKey ResourceKey, FVector TargetLocation)
{
	if (CurrentWorkerControlState != ESAS_WorkerControlState::ManualPersistent) return false;
	if (!CurrentHarvestResourceType) return false;

	CurrentHarvestResourceKey = ResourceKey;
	CurrentHarvestTargetLocation = TargetLocation;

	return true;
}

int32 USAS_WorkerControlComponent::GetBaseScoreForJob(const ESAS_MasterJobType JobType) const
{
	if (ActiveAssignment) return 0;
	if (JobType == ESAS_MasterJobType::None) return 0;

	switch (WorkerType)
	{
	case ESAS_WorkerType::Unspecified:
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Worker Assignment Type not assigned in WorkerControlComponent"));
		return 0;

	case ESAS_WorkerType::Villager:
		switch (JobType)
		{
		case ESAS_MasterJobType::DeliverItem:
			return 1;

		case ESAS_MasterJobType::RemoveItem:
			return 1;

		case ESAS_MasterJobType::Build:
			return 5;

		default:
			return 0;

		}
	case ESAS_WorkerType::Courier:
		switch (JobType)
		{
		case ESAS_MasterJobType::DeliverItem:
			return 5;

		case ESAS_MasterJobType::RemoveItem:
			return 5;
			
		case ESAS_MasterJobType::Build:
			return 0;

		default:
			return 0;
		}
	}

	return 0;
}

int32 USAS_WorkerControlComponent::GetCarryCapacityForItem(UItemDefinitionPrimaryData* Item) const
{
	if (!IsValid(Item)) return 0;

	if (!IsValid(WorkerInventory)) return 0;
	return WorkerInventory->GetAvailableInboundCapacity(Item, MAX_int32);

}

void USAS_WorkerControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		UnitInformation = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
		WorkerInventory = Owner->FindComponentByClass<USAS_InventoryComponent>();
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
		CurrentWorkerControlState = ESAS_WorkerControlState::LMQueue;
		LogisticsManager->RegisterAvailableWorker(this);
	}


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

bool USAS_WorkerControlComponent::IsAcceptingNewAssignments() const
{
	return ActiveAssignment == nullptr && CurrentWorkerControlState == ESAS_WorkerControlState::LMQueue;
}
