


#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h"
#include "Misc/Structs/SAS_WA_FailureContext.h"
#include "Misc/Structs/SAS_LogisticsJobWidgetInfo.h"
#include "Core/Components/SAS_WorkerControlComponent.h"

void USAS_LogisticsMasterJob::InitializeJob(USAS_LogisticsManagerComponent* InOwningLM, ESAS_MasterJobType InJobType, int32 InPriority, AActor* InRequestingActor)
{
	MasterJobID = FGuid::NewGuid();
	OwningLM = InOwningLM;
	MasterJobType = InJobType;
	Priority = InPriority;
	RequestingActor = InRequestingActor;
	MasterJobStatus = ESAS_MasterJobStatus::Pending;

	JobInitialized = true;
}

void USAS_LogisticsMasterJob::AddAssignment(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;

	ActiveAssignments.AddUnique(Assignment);
}

void USAS_LogisticsMasterJob::NotifyAssignmentCompleted(USAS_LogisticsWorkerAssignment* Assignment)
{
	RemoveAssignment(Assignment);
	if (EvaluateIsComplete()) 
	{
		MarkCompleted();
	}
}

void USAS_LogisticsMasterJob::NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* Assignment)
{
	RemoveAssignment(Assignment);
}

void USAS_LogisticsMasterJob::NotifyAssignmentFailed(USAS_LogisticsWorkerAssignment* Assignment, FSAS_WA_FailureContext FailureContext)
{
	if (!IsValid(Assignment)) return;
	if (FailureContext.bRejectWorkerForThisMasterJob)
	{
		WorkersToIgnore.AddUnique(Assignment->AssignedWorker);
	}

	RemoveAssignment(Assignment);
	//Will need to re-post assignment and log failure / failure reason
}

void USAS_LogisticsMasterJob::MarkCancelled()
{
	if (IsFinished()) return;
	MasterJobStatus = ESAS_MasterJobStatus::Cancelled;

	OnLogisticsMasterJobUpdated.Broadcast(this);

	//Need to add in logic to allow for a certain amount of failures before this job becomes inactive.
	if (OwningLM)
	{
		//TODO: Set up LM level cancellation of a master level job
	}
}

bool USAS_LogisticsMasterJob::IsFinished() const
{
	return MasterJobStatus == ESAS_MasterJobStatus::Completed || MasterJobStatus == ESAS_MasterJobStatus::Failed || MasterJobStatus == ESAS_MasterJobStatus::Cancelled;
}

FSAS_LogisticsJobWidgetInfo USAS_LogisticsMasterJob::GetJobInfoForWidget() const
{
	FSAS_LogisticsJobWidgetInfo WidgetInfo;
	
	WidgetInfo.RequestingActor = RequestingActor;
	WidgetInfo.JobType = MasterJobType;
	WidgetInfo.JobStatus = MasterJobStatus;
	
	return WidgetInfo;
}

bool USAS_LogisticsMasterJob::WorkerIsAcceptable(USAS_WorkerControlComponent* Worker)
{
	return !WorkersToIgnore.Contains(Worker);
}

void USAS_LogisticsMasterJob::RemoveAssignment(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;
	ActiveAssignments.Remove(Assignment);
	//Not really enough for any job. Will be updated in all children
}

bool USAS_LogisticsMasterJob::EvaluateIsComplete() const
{
	return false;
}

void USAS_LogisticsMasterJob::MarkCompleted()
{
	if (IsFinished()) return;
	MasterJobStatus = ESAS_MasterJobStatus::Completed;
	
	OnLogisticsMasterJobUpdated.Broadcast(this);

	if (OwningLM)
	{
		//TODO: Set up LM level closout of a master level job. Possibly use the broadcast above?
	}
}

void USAS_LogisticsMasterJob::MarkFailed()
{
	if (IsFinished()) return;
	MasterJobStatus = ESAS_MasterJobStatus::Failed;

	OnLogisticsMasterJobUpdated.Broadcast(this);

	if (OwningLM)
	{
		//TODO: Set up LM level cancellation of a master level job
	}
}