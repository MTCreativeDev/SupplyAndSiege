


#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h"

void USAS_LogisticsMasterJob::InitializeJob(USAS_LogisticsManagerComponent* InOwningLM, ESAS_MasterJobType InJobType, int32 InPriority, AActor* InRequestingActor)
{
	MasterJobID = FGuid::NewGuid();
	OwningLM = InOwningLM;
	MasterJobType = InJobType;
	Priority = InPriority;
	RequestingActor = InRequestingActor;
	MasterJobStatus = ESAS_MasterJobStatus::Pending;
}

void USAS_LogisticsMasterJob::AddAssignment(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;

	ActiveAssignments.AddUnique(Assignment);

	if (MasterJobStatus == ESAS_MasterJobStatus::Pending) 
	{
		MasterJobStatus = ESAS_MasterJobStatus::Active;
	}
	//Will need to update for item delivery types since those will need to evaluate if all items are being delivered.
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
	//Will need to re-post assignment
}

void USAS_LogisticsMasterJob::NotifyAssignmentFailed(USAS_LogisticsWorkerAssignment* Assignment)
{
	RemoveAssignment(Assignment);
	//Will need to re-post assignment and log failure / failure reason
}

void USAS_LogisticsMasterJob::MarkCancelled()
{
	if (IsFinished()) return;
	MasterJobStatus = ESAS_MasterJobStatus::Cancelled;
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
	//Will need to check in children. Transport jobs will likely have multiple assignments
	if (OwningLM)
	{
		//TODO: Set up LM level closout of a master level job
	}
}

void USAS_LogisticsMasterJob::MarkFailed()
{
	if (IsFinished()) return;
	MasterJobStatus = ESAS_MasterJobStatus::Failed;
	if (OwningLM)
	{
		//TODO: Set up LM level cancellation of a master level job
	}
}