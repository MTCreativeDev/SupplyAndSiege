


#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Core/Objects/LWAs/SAS_LWA_TransportItems.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Core/Actors/SAS_BuildingLayout.h"
#include "Core/Actors/SAS_BL_BuildJob.h"
#include "Core/Components/SAS_WorkerControlComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/Structs/SAS_LogisticsOffering.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"


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

	TArray<FSAS_LogisticsOfferingGroup> GroupedOfferings;
	GetOfferingsGroupedBySource(GroupedOfferings);
	NotifyGroupedLogisticsOfferingsUpdated.Broadcast(GroupedOfferings);


	TryAssignJobs();
}

TArray<USAS_WorkerControlComponent*> USAS_LogisticsManagerComponent::GetAvailableWorkers() const
{
	TArray<USAS_WorkerControlComponent*> Result;
	for (USAS_WorkerControlComponent* Worker : AvailableWorkers)
	{
		if (IsValid(Worker))
		{
			Result.Add(Worker);
		}
	}
	return Result;
}

void USAS_LogisticsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void USAS_LogisticsManagerComponent::GetOfferingsGroupedBySource(TArray<FSAS_LogisticsOfferingGroup>& OutGroups) const
{
	OutGroups.Reset();

	TMap<USAS_InventoryComponent*, int32> GroupIndexBySource;

	for (const FSAS_LogisticsOffering& Offer : ActiveOfferings)
	{
		if (!IsValid(Offer.SourceInventory)) continue;
		if (!IsValid(Offer.Item)) continue;
		if (Offer.Quantity <= 0) continue;

		int32* ExistingIndex = GroupIndexBySource.Find(Offer.SourceInventory);
		int32 GroupIndex = INDEX_NONE;

		if (ExistingIndex)
		{
			GroupIndex = *ExistingIndex;
		}
		else
		{
			GroupIndex = OutGroups.AddDefaulted();
			GroupIndexBySource.Add(Offer.SourceInventory, GroupIndex);
			OutGroups[GroupIndex].SourceInventory = Offer.SourceInventory;
		}

		FSAS_LogisticsOfferingEntry NewEntry;
		NewEntry.Item = Offer.Item;
		NewEntry.Quantity = Offer.Quantity;

		OutGroups[GroupIndex].OfferedItems.Add(NewEntry);
	}
}

void USAS_LogisticsManagerComponent::HandleLogisticsMasterJobUpdated(USAS_LogisticsMasterJob* LogisticsMasterJob)
{
	NotifyLogisticsMasterJobUpdated.Broadcast(LogisticsMasterJob);
}

void USAS_LogisticsManagerComponent::TryAssignJobs()
{
	ActiveOfferings.RemoveAll([](const FSAS_LogisticsOffering& Offer)
		{
			return !IsValid(Offer.SourceInventory) || !IsValid(Offer.Item) || Offer.Quantity <= 0;
		});

	AvailableWorkers.RemoveAll([](const TObjectPtr<USAS_WorkerControlComponent>& W)
		{
			return !IsValid(W);
		});

	ActiveJobs.RemoveAll([](const TObjectPtr<USAS_LogisticsMasterJob>& Job)
		{
			return !IsValid(Job);
		});

	if (ActiveJobs.Num() == 0) return;
	if (AvailableWorkers.Num() == 0) return;

	TArray<TObjectPtr<USAS_LogisticsMasterJob>> SortedJobs = ActiveJobs;

	SortedJobs.Sort([](const TObjectPtr<USAS_LogisticsMasterJob>& A, const TObjectPtr<USAS_LogisticsMasterJob>& B)
		{
			const USAS_LogisticsMasterJob* JobA = A.Get();
			const USAS_LogisticsMasterJob* JobB = B.Get();

			if (!IsValid(JobA)) return false;
			if (!IsValid(JobB)) return true;

			return JobA->Priority > JobB->Priority;
		});

	TArray<TObjectPtr<USAS_WorkerControlComponent>> RemainingWorkers = AvailableWorkers;

	for (const TObjectPtr<USAS_LogisticsMasterJob>& ActiveJob : SortedJobs)
	{
		USAS_LogisticsMasterJob* Job = ActiveJob.Get();

		if (!IsValid(Job)) continue;

		if (Job->MasterJobStatus != ESAS_MasterJobStatus::Pending) continue;

		USAS_WorkerControlComponent* BestWorker = nullptr;
		FSAS_LogisticsOffering BestOffering = FSAS_LogisticsOffering();
		int32 AssignedAmount = 0;

		switch (Job->MasterJobType)
		{
		case ESAS_MasterJobType::DeliverItem:
		{
			USAS_LMJ_DeliverItem* DeliverJob = Cast<USAS_LMJ_DeliverItem>(Job);
			if (!IsValid(DeliverJob)) break;

			if (!FindBestTransportCandidate(DeliverJob,RemainingWorkers, BestWorker, BestOffering, AssignedAmount))
			{
				break;
			}

			if (!IsValid(BestWorker) || !IsValid(BestOffering.SourceInventory) || !IsValid(DeliverJob->RequestingActor))
			{
				break;
			}

			UItemDefinitionPrimaryData* ItemDefinition = DeliverJob->GetItemDefinition();
			if (!IsValid(ItemDefinition))
			{
				break;
			}

			AActor* SourceActor = BestOffering.SourceInventory->GetOwner();
			AActor* TargetActor = DeliverJob->RequestingActor;

			if (!IsValid(SourceActor) || !IsValid(TargetActor))
			{
				break;
			}

			USAS_LWA_TransportItems* NewAssignment = NewObject<USAS_LWA_TransportItems>(this);
			if (!IsValid(NewAssignment)) break;

			NewAssignment->InitializeTransportAssignment(
				DeliverJob,
				BestWorker,
				ItemDefinition,
				AssignedAmount,
				SourceActor,
				TargetActor
			);

			if (BestWorker->TryAcceptAssignment(NewAssignment))
			{
				DeliverJob->AddAssignment(NewAssignment);
				NewAssignment->StartAssignment();

				RemainingWorkers.RemoveAll([BestWorker](const TObjectPtr<USAS_WorkerControlComponent>& W)
					{
						return W == BestWorker;
					});
			}

			break;
		}

		default:
			break;
		}
	}
}

bool USAS_LogisticsManagerComponent::FindBestTransportCandidate(USAS_LMJ_DeliverItem* DeliveryJob, const TArray<TObjectPtr<USAS_WorkerControlComponent>>& WorkersToConsider, USAS_WorkerControlComponent*& OutBestWorker, FSAS_LogisticsOffering& OutBestOffering, int32& OutAssignedAmount)
{
	OutBestWorker = nullptr;
	OutBestOffering = FSAS_LogisticsOffering();
	OutAssignedAmount = 0;

	if (!IsValid(DeliveryJob)) return false;

	UItemDefinitionPrimaryData* NeededItem = DeliveryJob->GetItemDefinition();
	const int32 NeededAmount = DeliveryJob->GetUnreservedAmount();

	if (!IsValid(NeededItem)) return false;
	if (NeededAmount <= 0) return false;

	int32 BestScore = -1;

	for (USAS_WorkerControlComponent* Worker : WorkersToConsider)
	{
		if (!IsValid(Worker)) continue;
		if (!DeliveryJob->WorkerIsAcceptable(Worker)) continue;

		const int32 BaseScore = Worker->GetBaseScoreForJob(ESAS_MasterJobType::DeliverItem);
		if (BaseScore <= 0) continue;

		const int32 WorkerCapacity = Worker->GetCarryCapacityForItem(NeededItem);
		if (WorkerCapacity <= 0) continue;

		for (const FSAS_LogisticsOffering& Offering : ActiveOfferings)
		{
			if (!IsValid(Offering.SourceInventory)) continue;
			if (!IsValid(Offering.Item)) continue;
			if (Offering.Item != NeededItem) continue;
			if (Offering.Quantity <= 0) continue;

			const int32 AssignedAmount = FMath::Min3(
				NeededAmount,
				WorkerCapacity,
				Offering.Quantity
			);

			if (AssignedAmount <= 0) continue;

			int32 Score = BaseScore;

			if (AssignedAmount == NeededAmount)
			{
				Score += 1000; // High score for any worker provider combo that finishes the job entirely
			}
			else if (AssignedAmount == WorkerCapacity)
			{
				Score += 500; // Not as high, but still higher score if the combination of the worker and depot fills the workers inventory
			}

			Score += AssignedAmount;

			AActor* WorkerActor = Worker->GetOwner();
			if (!IsValid(WorkerActor)) continue;

			AActor* SourceActor = Offering.SourceInventory->GetOwner();
			if (!IsValid(SourceActor)) continue;

			AActor* TargetActor = DeliveryJob->RequestingActor;
			if (!IsValid(TargetActor)) continue;

			FVector SourceLocation = SourceActor->GetActorLocation();

			if (ASAS_BuildingLayout* SourceBuilding = Cast<ASAS_BuildingLayout>(SourceActor))
			{
				SourceLocation = SourceBuilding->GetBestMoveToWorldLocation(WorkerActor->GetActorLocation());
			}

			const float WorkerToSourceDistance = FVector::Dist(
				WorkerActor->GetActorLocation(),
				SourceLocation
			);

			float SourceToTargetPathLength = 0.f;
			if (!GetCachedOrComputePathLength(SourceActor, TargetActor, SourceToTargetPathLength))
			{
				SourceToTargetPathLength = FVector::Dist(
					SourceActor->GetActorLocation(),
					TargetActor->GetActorLocation()
				);
			}

			const int32 WorkerDistancePenalty = FMath::RoundToInt(WorkerToSourceDistance * 0.01f);
			const int32 SourceTargetPenalty = FMath::RoundToInt(SourceToTargetPathLength * 0.005f);

			Score -= WorkerDistancePenalty;
			Score -= SourceTargetPenalty;



			if (Score > BestScore)
			{
				BestScore = Score;
				OutBestWorker = Worker;
				OutBestOffering = Offering;
				OutAssignedAmount = AssignedAmount;
			}
		}
	}

	return IsValid(OutBestWorker) && IsValid(OutBestOffering.SourceInventory) && OutAssignedAmount > 0;
}

bool USAS_LogisticsManagerComponent::GetCachedOrComputePathLength(AActor* SourceActor, AActor* TargetActor, float& OutPathLength)
{
	OutPathLength = 0.f;

	if (!IsValid(SourceActor) || !IsValid(TargetActor)) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	FSAS_SourceTargetPair Key;
	Key.Source = SourceActor;
	Key.Target = TargetActor;

	const float CurrentTime = World->GetTimeSeconds();
	const float MaxCacheAge = 300.f;

	if (FSAS_PathDistanceCache* Cached = CachedPathDistances.Find(Key))
	{
		if ((CurrentTime - Cached->LastValidatedTime) <= MaxCacheAge)
		{
			OutPathLength = Cached->PathLength;
			return true;
		}
	}

	FSAS_SourceTargetPair InvertedKey;
	InvertedKey.Source = TargetActor;
	InvertedKey.Target = SourceActor;

	if (FSAS_PathDistanceCache* Cached = CachedPathDistances.Find(InvertedKey))
	{
		if ((CurrentTime - Cached->LastValidatedTime) <= MaxCacheAge)
		{
			OutPathLength = Cached->PathLength;
			return true;
		}
	}

	FVector SourceLocation = SourceActor->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	if (ASAS_BuildingLayout* SourceBuilding = Cast<ASAS_BuildingLayout>(SourceActor))
	{
		SourceLocation = SourceBuilding->GetBestMoveToWorldLocation(TargetActor->GetActorLocation());
	}

	if (ASAS_BuildingLayout* TargetBuilding = Cast<ASAS_BuildingLayout>(TargetActor))
	{
		TargetLocation = TargetBuilding->GetBestMoveToWorldLocation(SourceLocation);
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	if (!NavSys) return false;

	ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (!NavData) return false;

	FPathFindingQuery Query;
	Query.StartLocation = SourceLocation;
	Query.EndLocation = TargetLocation;
	Query.NavData = NavData;

	FPathFindingResult Result = NavSys->FindPathSync(Query);

	if (!Result.IsSuccessful() || !Result.Path.IsValid()) return false;

	OutPathLength = Result.Path->GetLength();

	FSAS_PathDistanceCache NewCache;
	NewCache.PathLength = OutPathLength;
	NewCache.LastValidatedTime = CurrentTime;

	CachedPathDistances.Add(Key, NewCache);

	return true;
}

TArray<USAS_LogisticsMasterJob*> USAS_LogisticsManagerComponent::CreateBuildSiteResourceDeliveryJob(const FSAS_ResourceDeliveryRequest& ResourceDeliveryInformation)
{
	TArray<USAS_LogisticsMasterJob*> CreatedJobs;

	if (!ResourceDeliveryInformation.IsValid()) return CreatedJobs;

	for (const TPair<TObjectPtr<UItemDefinitionPrimaryData>, int32>& Pair : ResourceDeliveryInformation.ResourceCost)
	{
		USAS_LMJ_DeliverItem* NewJob = NewObject<USAS_LMJ_DeliverItem>(this);
		if (!IsValid(NewJob)) continue;

		NewJob->InitializeDeliverItemJob(this, ResourceDeliveryInformation.BuildSite, Pair.Key, Pair.Value, ResourceDeliveryInformation.Priority);
		NewJob->OnLogisticsMasterJobUpdated.AddUObject(this, &USAS_LogisticsManagerComponent::HandleLogisticsMasterJobUpdated);

		ActiveJobs.Add(NewJob);
		CreatedJobs.Add(NewJob);

		NotifyLogisticsMasterJobUpdated.Broadcast(NewJob);
	}

	TryAssignJobs();

	return CreatedJobs;
}

void USAS_LogisticsManagerComponent::RegisterAvailableWorker(USAS_WorkerControlComponent* Worker)
{
	if (!IsValid(Worker)) return;

	AvailableWorkers.AddUnique(Worker);
	NotifyAvailableWorkersUpdated.Broadcast(GetAvailableWorkers());

	TryAssignJobs();
}

void USAS_LogisticsManagerComponent::UnregisterAvailableWorker(USAS_WorkerControlComponent* Worker)
{
	if (!IsValid(Worker)) return;
	AvailableWorkers.Remove(Worker);


	NotifyAvailableWorkersUpdated.Broadcast(GetAvailableWorkers());
}