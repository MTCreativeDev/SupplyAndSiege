


#include "Core/Objects/LWAs/SAS_LWA_TransportItems.h"
#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Misc/Structs/SAS_WA_FailureContext.h"

void USAS_LWA_TransportItems::InitializeTransportAssignment(USAS_LogisticsMasterJob* InParentMasterJob, USAS_UnitInformationComponent* InAssignedWorker, UItemDefinitionPrimaryData* InItemDefinition, int32 InAssignedAmount, AActor* InSourceActor, AActor* InTargetActor)
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
	Super::StartAssignment();

	if (IsFinished()) return;

	//TODO: Any starting logic for this job type.
	AdvanceAssignment();
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
		//TODO:: handle inventory in inventory and release inbound reservation only. Need to release the target inventory reservation as well
	}
	Super::FailAssignment(FailureContext);
}

void USAS_LWA_TransportItems::AdvanceAssignment()
{
	if (IsFinished()) return;

	switch (CurrentPhase)
	{
	case ESAS_TransportAssignmentPhase::Entry:
	{	
		if (!SourceActor)
		{
			FSAS_WA_FailureContext FailureContext;
			FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::InvalidSourceActor;
			FailureContext.Worker = AssignedWorker;
			FailureContext.bRejectWorkerForThisMasterJob = true;
			FailureContext.bReEvaluateMasterJobValidity = true;
			FailureContext.bEditorDebug = true;

			FailAssignment(FailureContext);
			return;
		}

		if (!TargetActor)
		{
			FSAS_WA_FailureContext FailureContext;
			FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::InvlidTargetActor;
			FailureContext.Worker = AssignedWorker;
			FailureContext.bRejectWorkerForThisMasterJob = true;
			FailureContext.bReEvaluateMasterJobValidity = true;
			FailureContext.bEditorDebug = true;

			FailAssignment(FailureContext);
			return;
		}

		if (!ItemDefinition)
		{
			FSAS_WA_FailureContext FailureContext;
			FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::InvalidItemDefinition;
			FailureContext.Worker = AssignedWorker;
			FailureContext.bRejectWorkerForThisMasterJob = true;
			FailureContext.bReEvaluateMasterJobValidity = true;
			FailureContext.bEditorDebug = true;

			FailAssignment(FailureContext);
			return;
		}

		if (AssignedAmount <= 0)
		{
			FSAS_WA_FailureContext FailureContext;
			FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::InvalidItemAmount;
			FailureContext.Worker = AssignedWorker;
			FailureContext.bRejectWorkerForThisMasterJob = true;
			FailureContext.bReEvaluateMasterJobValidity = true;
			FailureContext.bEditorDebug = true;

			FailAssignment(FailureContext);
			return;
		}

		//TODO: Validate can move to the source. If not, fail

		if (!ReserveOutboundInventory())
		{
			FSAS_WA_FailureContext FailureContext;
			FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::FailedReservationOutbound;
			FailureContext.Worker = AssignedWorker;
			FailureContext.SourceActor = SourceActor;
			FailureContext.bRejectWorkerForThisMasterJob = false;
			FailureContext.bReEvaluateMasterJobValidity = false;
			FailureContext.bEditorDebug = false;

			FailAssignment(FailureContext);
			return;
		}

		//WARNING: Make sure the Reserving the OutboundInventory updates the assigned amount. If the unit can only get x amount of inventory, it should be the new assigned amount and should be the number used to update the inbound reservation.

		if (!ReserveInboundInventory())
		{
			FSAS_WA_FailureContext FailureContext;
			FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::FailedReservationInbound;
			FailureContext.Worker = AssignedWorker;
			FailureContext.TargetActor = TargetActor;
			FailureContext.bRejectWorkerForThisMasterJob = false;
			FailureContext.bReEvaluateMasterJobValidity = false;
			FailureContext.bEditorDebug = false;

			FailAssignment(FailureContext);
			return;
		}

		CurrentPhase = ESAS_TransportAssignmentPhase::MovingToSource;
		break;

	}

	case ESAS_TransportAssignmentPhase::MovingToSource:
	{
		//TODO
	}

	case ESAS_TransportAssignmentPhase::PickingUp:
	{
		//TODO:
	}

	case ESAS_TransportAssignmentPhase::MovingToTarget:
	{
		//TODO:
	}

	case ESAS_TransportAssignmentPhase::DroppingOff:
	{
		//TODO:
	}

	default:
	{
		FSAS_WA_FailureContext FailureContext;
		FailureContext.Reason = ESAS_WorkerAssignmentFailureReason::Unknown;
		FailureContext.Worker = AssignedWorker;
		FailureContext.bRejectWorkerForThisMasterJob = true;
		FailureContext.bEditorDebug = true;
		FailureContext.FailureDescription = FText::FromString(TEXT("Reacehd the default switch statement in the AdvanceAssignment function within the SAS_LWA_TransportItems class"));

		FailAssignment(FailureContext);
	}


	}
}

bool USAS_LWA_TransportItems::ReserveOutboundInventory()
{
	return false;
}

bool USAS_LWA_TransportItems::ReserveInboundInventory()
{
	return false;
}

void USAS_LWA_TransportItems::ReleaseReservations()
{
}

bool USAS_LWA_TransportItems::PickupReservation()
{
	return false;
}

bool USAS_LWA_TransportItems::DropoffReservation()
{
	return false;
}



