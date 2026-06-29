


#include "Core/Objects/LWAs/SAS_LWA_TransportItems.h"
#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Misc/Structs/SAS_WA_FailureContext.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Core/SAS_GameplayTagContainer.h"

void USAS_LWA_TransportItems::InitializeTransportAssignment(USAS_LogisticsMasterJob* InParentMasterJob, USAS_WorkerControlComponent* InAssignedWorker, UItemDefinitionPrimaryData* InItemDefinition, int32 InAssignedAmount, AActor* InSourceActor, AActor* InTargetActor)
{
	InitializeAssignment(InParentMasterJob, InAssignedWorker, ESAS_WorkerAssignmentType::TransportItems);
	ItemDefinition = InItemDefinition;
	AssignedAmount = InAssignedAmount;
	SourceActor = InSourceActor;
	TargetActor = InTargetActor;

	CurrentPhase = ESAS_TransportAssignmentPhase::Entry;
	bPickedUpReservation = false;

	OutboundReservationHandle.Reset();
	InboundReservationHandle.Reset();
}

void USAS_LWA_TransportItems::StartAssignment()
{
	if (IsFinished()) return;

	Super::StartAssignment();
	if (RunEntryChecks())
	{
		RequestMoveToSource();
	}
}

void USAS_LWA_TransportItems::CancelAssignment()
{
	if (IsFinished()) return;
	if (!bPickedUpReservation)
	{
		ReleaseReservations();
	}
	else
	{
		//TODO:: handle inventory in inventory and release inbound reservation only. Need to release the target inventory reservation as well
	}
	Super::CancelAssignment();

}

void USAS_LWA_TransportItems::FailAssignment(FSAS_WA_FailureContext FailureContext)
{
	if (IsFinished()) return;
	if (!bPickedUpReservation)
	{
		ReleaseReservations();
	}
	else
	{
		//TODO:: handle items in inventory and release inbound reservation
	}
	Super::FailAssignment(FailureContext);
}

void USAS_LWA_TransportItems::CompleteAssignment()
{
	if (IsFinished()) return;

	ReleaseReservations();

	Super::CompleteAssignment();
}

bool USAS_LWA_TransportItems::ReserveOutboundInventory()
{
	if (!SourceActor || !ItemDefinition || AssignedAmount <= 0) return false;

	USAS_InventoryComponent* SourceInventory = SourceActor->FindComponentByClass<USAS_InventoryComponent>();
	if (!SourceInventory) return false;

	int32 ReservedQuantity = 0;
	if (!SourceInventory->ReserveOutbound(ItemDefinition, AssignedAmount, this, OutboundReservationHandle, ReservedQuantity))
	{
		OutboundReservationHandle.Reset();
		return false;
	}

	if (ReservedQuantity != AssignedAmount)
	{
		//TODO: Update so this can do partials. Would need to update the LMJ
		SourceInventory->ReleaseReservation(OutboundReservationHandle);
		OutboundReservationHandle.Reset();
		return false;
	}

	return true;
}

bool USAS_LWA_TransportItems::ReserveInboundInventory()
{
	
 
	if (!TargetActor || !ItemDefinition || AssignedAmount <= 0) return false;

	USAS_InventoryComponent* TargetInventory = TargetActor->FindComponentByClass<USAS_InventoryComponent>();
	if (!TargetInventory) return false;

	int32 ReservedQuantity = 0;
	if (!TargetInventory->ReserveInbound(ItemDefinition, AssignedAmount, this, InboundReservationHandle, ReservedQuantity))
	{
		InboundReservationHandle.Reset();
		return false;
	}

	if (ReservedQuantity != AssignedAmount)
	{
		//TODO: Update so this can do partials. Would need to update the LMJ
		TargetInventory->ReleaseReservation(InboundReservationHandle);
		InboundReservationHandle.Reset();
		return false;
	}

	return true;

}

void USAS_LWA_TransportItems::ReleaseReservations()
{
	if (OutboundReservationHandle.IsValid())
	{
		if (SourceActor)
		{
			if (USAS_InventoryComponent* SourceInventory = SourceActor->FindComponentByClass<USAS_InventoryComponent>())
			{
				SourceInventory->ReleaseReservation(OutboundReservationHandle);
			}
		}

		OutboundReservationHandle.Reset();
	}

	if (InboundReservationHandle.IsValid())
	{
		if (TargetActor)
		{
			if (USAS_InventoryComponent* TargetInventory = TargetActor->FindComponentByClass<USAS_InventoryComponent>())
			{
				TargetInventory->ReleaseReservation(InboundReservationHandle);
			}
		}

		InboundReservationHandle.Reset();
	}
}

bool USAS_LWA_TransportItems::PickupReservation()
{
	if (!SourceActor || !AssignedWorker || !OutboundReservationHandle.IsValid()) return false;

	USAS_InventoryComponent* SourceInventory = SourceActor->FindComponentByClass<USAS_InventoryComponent>();
	if (!SourceInventory) return false;

	USAS_InventoryComponent* WorkerInventory = AssignedWorker->GetWorkerInventoryComponent();
	if (!WorkerInventory) return false;

	int32 RemovedQuantity = 0;
	if (!SourceInventory->FinalizeOutboundPickup(OutboundReservationHandle, RemovedQuantity))
	{
		return false;
	}

	OutboundReservationHandle.Reset();

	if (RemovedQuantity <= 0) return false;

	const int32 AddedQuantity = WorkerInventory->AddItem(ItemDefinition, RemovedQuantity);
	if (AddedQuantity != RemovedQuantity)
	{
		//TODO:: Need to have extra items grabbed dropped on the ground. Otherwise items get deleted.

		//TODO:: In the future, if this happens we can continue, but need to update the job reservation and the inventory reservation at the destination. Easire to just cancel though.

		bPickedUpReservation = AddedQuantity > 0;
		return false;
	}

	bPickedUpReservation = true;
	return true;
}

void USAS_LWA_TransportItems::RequestMoveToTarget()
{
	if (IsFinished()) return;


	if (!AssignedWorker)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidWorker, true, true));
		return;
	}
	CurrentPhase = ESAS_TransportAssignmentPhase::MovingToTarget;
	AssignedWorker->SendStateTreeEvent(SASGameplayTags::StateTree_Worker_Logistics_DeliverItem_MoveToTarget);
}

bool USAS_LWA_TransportItems::DropoffReservation()
{
	if (!AssignedWorker || !TargetActor || !InboundReservationHandle.IsValid()) return false;

	USAS_InventoryComponent* WorkerInventory = AssignedWorker->GetWorkerInventoryComponent();
	if (!WorkerInventory) return false;

	USAS_InventoryComponent* TargetInventory = TargetActor->FindComponentByClass<USAS_InventoryComponent>();
	if (!TargetInventory) return false;

	const int32 RemovedQuantity = WorkerInventory->RemoveItem(ItemDefinition, AssignedAmount);
	if (RemovedQuantity <= 0) return false;

	int32 AddedQuantity = 0;
	if (!TargetInventory->FinalizeInboundDropoff(InboundReservationHandle, RemovedQuantity, AddedQuantity))
	{
		// TODO: Removed from worker but failed to add to target. Need fallback/drop item.
		return false;
	}

	InboundReservationHandle.Reset();

	if (AddedQuantity != RemovedQuantity)
	{
		// TODO: Partial dropoff. Need fallback/drop item.
		return false;
	}

	bPickedUpReservation = false;
	return true;
}

void USAS_LWA_TransportItems::NotifyWorkerActionAccepted()
{
	if (IsFinished()) return;

	switch (CurrentPhase)
	{
	case ESAS_TransportAssignmentPhase::MovingToSource:
	{
		if (!ReserveOutboundInventory())
		{
			FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::FailedReservationOutbound));
			return;
		}

		if (!ReserveInboundInventory())
		{
			FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::FailedReservationInbound));
			return;
		}
		break;
	}
	default:
		break;

	}
}

void USAS_LWA_TransportItems::NotifyWorkerActionFailed(ESAS_WorkerAssignmentFailureReason Reason)
{
	if (IsFinished()) return;

	FailAssignment(MakeFailure(Reason));
}

void USAS_LWA_TransportItems::NotifyWorkerActionCompleted()
{
	if (IsFinished()) return;

	switch (CurrentPhase)
	{
	case ESAS_TransportAssignmentPhase::MovingToSource:
	{
		CurrentPhase = ESAS_TransportAssignmentPhase::PickingUp;

		if (!PickupReservation())
		{
			FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::FailedPickup));
			return;
		}

		RequestMoveToTarget();
		break;
	}

	case ESAS_TransportAssignmentPhase::MovingToTarget:
	{
		CurrentPhase = ESAS_TransportAssignmentPhase::DroppingOff;

		if (!DropoffReservation())
		{
			FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::FailedDelivery));
			return;
		}

		CompleteAssignment();
		break;
	}

	default:
		break;
	}
}

void USAS_LWA_TransportItems::NotifyWorkerActionInterrupted(ESAS_WorkerAssignmentFailureReason Reason)
{
	if (IsFinished()) return;

	FailAssignment(MakeFailure(Reason, true, false, false));
}

bool USAS_LWA_TransportItems::RunEntryChecks()
{
	if (!AssignedWorker)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidWorker,true, true));
		return false;
	}

	if (!SourceActor)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidSourceActor,true, false, true));
		return false;
	}

	if (!TargetActor)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidTargetActor, true, false, true));
		return false;
	}

	if (!ItemDefinition)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidItemDefinition, true, false, true));
		return false;
	}

	if (AssignedAmount <= 0)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidItemAmount, true, false, true));
		return false;
	}

	USAS_InventoryComponent* Inventory = AssignedWorker->GetWorkerInventoryComponent();
	if (!Inventory)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidWorkerInventory, true, true, false));
		return false;
	}

	const int32 CarryCapacity = Inventory->GetAvailableInboundCapacity(ItemDefinition, AssignedAmount);

	if (CarryCapacity < AssignedAmount)
	{
		//Debug

		//End Debug

		if (CarryCapacity == 0)
		{
			FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidWorkerCarryCapacity, true, true, false));
			return false;
		}


		//TODO handle updating the job to tell it the new amount we are reserving. Once this is done, we can return true
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidWorkerCarryCapacity, true, true, false));
		return false;
	}

	return true;
}

void USAS_LWA_TransportItems::RequestMoveToSource()
{
	if (IsFinished()) return;


	if (!AssignedWorker)
	{
		FailAssignment(MakeFailure(ESAS_WorkerAssignmentFailureReason::InvalidWorker,true,true));
		return;
	}
	CurrentPhase = ESAS_TransportAssignmentPhase::MovingToSource;
	AssignedWorker->SendStateTreeEvent(SASGameplayTags::StateTree_Worker_Logistics_DeliverItem_MoveToSource);
}

FSAS_WA_FailureContext USAS_LWA_TransportItems::MakeFailure(ESAS_WorkerAssignmentFailureReason Reason, bool EditorDebug, bool RejectWorkerForThisMasterJob, bool ReEvaluateMasterJobValidity) const
{
	FSAS_WA_FailureContext FailureContext;
	FailureContext.Reason = Reason;
	FailureContext.Worker = AssignedWorker;
	FailureContext.SourceActor = SourceActor;
	FailureContext.TargetActor = TargetActor;
	FailureContext.bEditorDebug = EditorDebug;
	FailureContext.bReEvaluateMasterJobValidity = ReEvaluateMasterJobValidity;
	FailureContext.bRejectWorkerForThisMasterJob = RejectWorkerForThisMasterJob;
	return FailureContext;
}

