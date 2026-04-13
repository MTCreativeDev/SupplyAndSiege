


#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h" //Will likely want to replace with the specific child that handles deliveries
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Misc/Structs/SAS_WA_FailureContext.h"

void USAS_LMJ_DeliverItem::InitializeDeliverItemJob(USAS_LogisticsManagerComponent* InOwningLM, AActor* InRequestingActor, UItemDefinitionPrimaryData* InItemDefinition, int32 InRequestedAmount, int32 InPriority)
{
	InitializeJob(InOwningLM, ESAS_MasterJobType::DeliverItem, InPriority, InRequestingActor);

	ItemDefinition = InItemDefinition;
	RequestedAmount = FMath::Max(0, InRequestedAmount);
	DeliveredAmount = 0;
	ReservedAmount = 0;
}

void USAS_LMJ_DeliverItem::AddAssignment(USAS_LogisticsWorkerAssignment* Assignment)
{
	Super::AddAssignment(Assignment);

	if (!Assignment) return;

	//TODO: Need to get the Worker Assignment properly set up and then create it here.
}

void USAS_LMJ_DeliverItem::NotifyAssignmentCompleted(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;

	//TODO: Need to get the Worker Assignment properly set up and then create it here.

	RemoveAssignment(Assignment);
	//TODO: need to make sure this function updates the delivered amount and reserved amounts here.

	if (EvaluateIsComplete())
	{
		MarkCompleted();
	}
}

void USAS_LMJ_DeliverItem::NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;

	//TODO: Need to get the Worker Assignment properly set up and then create it here.

	RemoveAssignment(Assignment);

	//Likely need to update need
}

void USAS_LMJ_DeliverItem::NotifyAssignmentFailed(USAS_LogisticsWorkerAssignment* Assignment, FSAS_WA_FailureContext FailureContext)
{
	if (!Assignment) return;

	//TODO: Need to get the Worker Assignment properly set up and then create it here.

	RemoveAssignment(Assignment);

	//Likely need to update need
	//Want to log the failure. This should be visible in UI
}

int32 USAS_LMJ_DeliverItem::GetRemainingAmount() const
{
	return FMath::Max(0, RequestedAmount - DeliveredAmount);
}

int32 USAS_LMJ_DeliverItem::GetUnreservedAmount() const
{
	return FMath::Max(0, RequestedAmount - DeliveredAmount - ReservedAmount);
}

bool USAS_LMJ_DeliverItem::CanAcceptMoreAssignments() const
{
	return !IsFinished() && GetUnreservedAmount() > 0;
}

bool USAS_LMJ_DeliverItem::EvaluateIsComplete() const
{
	return DeliveredAmount >= RequestedAmount;
}
