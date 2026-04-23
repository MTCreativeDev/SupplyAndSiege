


#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Core/Actors/SAS_BL_BuildJob.h"
#include "Core/Components/SAS_WorkerControlComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"


USAS_LogisticsManagerComponent::USAS_LogisticsManagerComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void USAS_LogisticsManagerComponent::UpdateInventoryOfferings(USAS_InventoryComponent* SourceInventory, const TArray<FSAS_LogisticsOffering>& NewOfferings)
{
	if (!IsValid(SourceInventory)) return;

	ActiveOfferings.RemoveAll([SourceInventory](const FSAS_LogisticsOffering& Offer)
		{
			return Offer.SourceInventory == SourceInventory;
		});

	for (const FSAS_LogisticsOffering& Offer : NewOfferings)
	{
		if (!IsValid(Offer.SourceInventory)) continue;
		if (!IsValid(Offer.Item)) continue;
		if (Offer.Quantity <= 0) continue;

		ActiveOfferings.Add(Offer);
	}

	//DEBUG
	if (GEngine)
	{
		const FString SourceName = SourceInventory->GetOwner() ? SourceInventory->GetOwner()->GetName() : TEXT("NoOwner");

		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Green,
			FString::Printf(TEXT("Updating Offerings | Source: %s | New Count: %d"), *SourceName, NewOfferings.Num())
		);
	}

}

void USAS_LogisticsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USAS_LogisticsManagerComponent::HandleLogisticsMasterJobUpdated(USAS_LogisticsMasterJob* LogisticsMasterJob)
{
	NotifyLogisticsMasterJobUpdated.Broadcast(LogisticsMasterJob);
}

void USAS_LogisticsManagerComponent::TryAssignJobs()
{
	if (ActiveJobs.Num() == 0) return;
	if (AvailableWorkers.Num() == 0) return;

	TArray<TObjectPtr<USAS_LogisticsMasterJob>> SortedJobs = ActiveJobs;
	SortedJobs.Sort([](const USAS_LogisticsMasterJob& A, const USAS_LogisticsMasterJob& B)
		{
			return A.Priority > B.Priority;
		});

	TArray<TObjectPtr<USAS_WorkerControlComponent>> RemainingWorkers = AvailableWorkers;

	for (TObjectPtr<USAS_LogisticsMasterJob> ActiveJob : SortedJobs)
	{
		if (!IsValid(ActiveJob)) continue;
		USAS_WorkerControlComponent* BestWorker = nullptr;
		int32 BestScore = -1;




		for (TObjectPtr<USAS_WorkerControlComponent> Worker : RemainingWorkers)
		{
			if (!IsValid(Worker)) continue;
			int32 BaseScore = Worker->GetBaseScoreForJob(ActiveJob->MasterJobType);
			if (BaseScore <= 0) continue;

			int32 TotalScore = BaseScore;
			//TODO Handle additional scoring logic like distance etc.

			if (TotalScore > BestScore)
			{
				BestScore = TotalScore;
				BestWorker = Worker;
			}
		}

		//Debug
		if (BestWorker && GEngine)
		{
			const FString WorkerName = BestWorker->GetOwner() ? BestWorker->GetOwner()->GetName() : TEXT("NoOwner");
			const FString WorkerTypeStr = UEnum::GetValueAsString(BestWorker->GetWorkerType());

			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Green,
				FString::Printf(TEXT("Best Worker: %s | Type: %s | Score: %d"), *WorkerName, *WorkerTypeStr, BestScore)
			);
		}

		if (BestWorker)
		{
			//TODO handle specific assignment logic for each job type
			RemainingWorkers.Remove(BestWorker);
		}

	}
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

	TryAssignJobs();
}

void USAS_LogisticsManagerComponent::RegisterAvailableWorker(USAS_WorkerControlComponent* Worker)
{
	if (!IsValid(Worker)) return;

	AvailableWorkers.AddUnique(Worker);

	TryAssignJobs();
}

void USAS_LogisticsManagerComponent::UnregisterAvailableWorker(USAS_WorkerControlComponent* Worker)
{
	if (!IsValid(Worker)) return;
	AvailableWorkers.Remove(Worker);
}



