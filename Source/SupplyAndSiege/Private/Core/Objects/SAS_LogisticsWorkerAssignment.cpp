


#include "Core/Objects/SAS_LogisticsWorkerAssignment.h"
#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Misc/Structs/SAS_WA_FailureContext.h"

void USAS_LogisticsWorkerAssignment::InitializeAssignment(USAS_LogisticsMasterJob* InParentMasterJob, USAS_WorkerControlComponent* InAssignedWorker, ESAS_WorkerAssignmentType InAssignmentType)
{
	WorkerAssignmentID = FGuid::NewGuid();
	ParentMasterJob = InParentMasterJob;
	AssignedWorker = InAssignedWorker;
	WorkerAssignmentType = InAssignmentType;
	WorkerAssignmentStatus = ESAS_WorkerAssignmentStatus::Pending;
}

void USAS_LogisticsWorkerAssignment::StartAssignment()
{
	if (IsFinished()) return;

	WorkerAssignmentStatus = ESAS_WorkerAssignmentStatus::Active;
}

void USAS_LogisticsWorkerAssignment::CancelAssignment()
{
	if (IsFinished()) return;

	WorkerAssignmentStatus = ESAS_WorkerAssignmentStatus::Cancelled;
	NotifyParentAssignmentCancelled();
}

void USAS_LogisticsWorkerAssignment::FailAssignment(FSAS_WA_FailureContext FailureContext)
{
	if (IsFinished()) return;

	WorkerAssignmentStatus = ESAS_WorkerAssignmentStatus::Failed;
	NotifyParentAssignmentFailed(FailureContext);
}

void USAS_LogisticsWorkerAssignment::CompleteAssignment()
{
	if (IsFinished()) return;

	WorkerAssignmentStatus = ESAS_WorkerAssignmentStatus::Completed;
	NotifyParentAssignmentCompleted();
}


bool USAS_LogisticsWorkerAssignment::IsFinished() const
{
	return WorkerAssignmentStatus == ESAS_WorkerAssignmentStatus::Completed || WorkerAssignmentStatus == ESAS_WorkerAssignmentStatus::Cancelled || WorkerAssignmentStatus == ESAS_WorkerAssignmentStatus::Failed;
}

void USAS_LogisticsWorkerAssignment::NotifyParentAssignmentCompleted()
{
	if (!ParentMasterJob) return;
	ParentMasterJob->NotifyAssignmentCompleted(this);
}

void USAS_LogisticsWorkerAssignment::NotifyParentAssignmentCancelled()
{
	if (!ParentMasterJob) return;
	ParentMasterJob->NotifyAssignmentCancelled(this);
}

void USAS_LogisticsWorkerAssignment::NotifyParentAssignmentFailed(FSAS_WA_FailureContext FailureContext)
{
	if (!ParentMasterJob) return;
	ParentMasterJob->NotifyAssignmentFailed(this, FailureContext);
}
