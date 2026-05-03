


#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h" //Will likely want to replace with the specific child that handles deliveries
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Misc/Structs/SAS_WA_FailureContext.h"
#include "Misc/Structs/SAS_LogisticsJobWidgetInfo.h"

void USAS_LMJ_DeliverItem::InitializeDeliverItemJob(USAS_LogisticsManagerComponent* InOwningLM, AActor* InRequestingActor, UItemDefinitionPrimaryData* InItemDefinition, int32 InRequestedAmount, int32 InPriority)
{
	if (JobInitialized) return;

	InitializeJob(InOwningLM, ESAS_MasterJobType::DeliverItem, InPriority, InRequestingActor);

	ItemDefinition = InItemDefinition;
	RequestedAmount = FMath::Max(0, InRequestedAmount);
	DeliveredAmount = 0;
	ReservedAmount = 0;
}

void USAS_LMJ_DeliverItem::AddAssignment(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;
	Super::AddAssignment(Assignment);
	
	ReservedAmount += Assignment->GetAssignedAmount();
	
	if (GetUnreservedAmount() <= 0)
	{
		MasterJobStatus = ESAS_MasterJobStatus::Active;
	}

	OnLogisticsMasterJobUpdated.Broadcast(this);
}

void USAS_LMJ_DeliverItem::NotifyAssignmentCompleted(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;

	DeliveredAmount += Assignment->GetAssignedAmount();
	ReservedAmount = FMath::Max(0, ReservedAmount - Assignment->GetAssignedAmount());

	Super::NotifyAssignmentCompleted(Assignment);

	OnLogisticsMasterJobUpdated.Broadcast(this);
}

void USAS_LMJ_DeliverItem::NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* Assignment)
{
	if (!Assignment) return;
	Super::NotifyAssignmentCancelled(Assignment);
	ReservedAmount = FMath::Max(0, ReservedAmount - Assignment->GetAssignedAmount());
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Cancelled"));


	if (GetUnreservedAmount() > 0)
	{
		MasterJobStatus = ESAS_MasterJobStatus::Pending;
	}

	OnLogisticsMasterJobUpdated.Broadcast(this);
}

void USAS_LMJ_DeliverItem::NotifyAssignmentFailed(USAS_LogisticsWorkerAssignment* Assignment, FSAS_WA_FailureContext FailureContext)
{
	if (!Assignment) return;

	Super::NotifyAssignmentFailed(Assignment, FailureContext);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Cancelled"));

	ReservedAmount = FMath::Max(0, ReservedAmount - Assignment->GetAssignedAmount());
	if (GetUnreservedAmount() > 0)
	{
		MasterJobStatus = ESAS_MasterJobStatus::Pending;
	}

	OnLogisticsMasterJobUpdated.Broadcast(this);
}

FSAS_LogisticsJobWidgetInfo USAS_LMJ_DeliverItem::GetJobInfoForWidget() const
{
	FSAS_LogisticsJobWidgetInfo WidgetInfo = Super::GetJobInfoForWidget();

	WidgetInfo.RequestedAmount = RequestedAmount;
	WidgetInfo.ItemDefinition = ItemDefinition;
	WidgetInfo.ReservedAmount = RequestedAmount - GetUnreservedAmount();
	WidgetInfo.DeliveredAmount = RequestedAmount - GetRemainingAmount();
	
	return WidgetInfo;
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
