


#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Core/Actors/SAS_BL_BuildJob.h"


USAS_LogisticsManagerComponent::USAS_LogisticsManagerComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}



void USAS_LogisticsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USAS_LogisticsManagerComponent::HandleLogisticsMasterJobUpdated(USAS_LogisticsMasterJob* LogisticsMasterJob)
{
	NotifyLogisticsMasterJobUpdated.Broadcast(LogisticsMasterJob);
}

void USAS_LogisticsManagerComponent::CreateBuildSiteResourceDeliveryJob(const FSAS_ResourceDeliveryRequest& ResourceDeliveryInformation)
{
	if (!ResourceDeliveryInformation.IsValid()) return;

	for (const TPair<TObjectPtr<UItemDefinitionPrimaryData>, int32>& Pair : ResourceDeliveryInformation.ResourceCost)
	{
		USAS_LMJ_DeliverItem* NewJob = NewObject<USAS_LMJ_DeliverItem>(this);
		NewJob->InitializeDeliverItemJob(this, ResourceDeliveryInformation.BuildSite, Pair.Key, Pair.Value, ResourceDeliveryInformation.Priority);
		NewJob->OnLogisticsMasterJobUpdated.AddUObject(this, &USAS_LogisticsManagerComponent::HandleLogisticsMasterJobUpdated);
		ActiveJobs.Add(NewJob);
		NotifyLogisticsMasterJobUpdated.Broadcast(NewJob);

	}
}



