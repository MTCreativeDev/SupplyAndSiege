

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "Misc/Structs/SAS_LogisticsOffering.h"
#include "Misc/Structs/SAS_LogisticsOfferingEntry.h"
#include "Misc/Structs/SAS_LogisticsOfferingGrouped.h"
#include "Misc/Structs/SAS_SourceTargetPair.h"
#include "Misc/Structs/SAS_PathDistanceCache.h"
#include "SAS_LogisticsManagerComponent.generated.h"

class USAS_LogisticsMasterJob;
class USAS_WorkerControlComponent;
class USAS_LMJ_DeliverItem;
class USAS_InventoryComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyLogisticsMasterJobUpdated, USAS_LogisticsMasterJob*, LogisticsMasterJob);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyGroupedLogisticsOfferingsUpdated, const TArray<FSAS_LogisticsOfferingGroup>&, OfferingGroups);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyAvailableWorkersUpdated, const TArray<USAS_WorkerControlComponent*>&, Workers);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_LogisticsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_LogisticsManagerComponent();

	TArray<USAS_LogisticsMasterJob*> CreateBuildSiteResourceDeliveryJob(const FSAS_ResourceDeliveryRequest& ResourceDeliveryInformation);

	UFUNCTION(BlueprintCallable, Category = "Logistics")
	TArray<USAS_LogisticsMasterJob*> GetActiveJobs() const { return ActiveJobs; }

	void RegisterAvailableWorker(USAS_WorkerControlComponent* Worker);
	void UnregisterAvailableWorker(USAS_WorkerControlComponent* Worker);

	UFUNCTION(BlueprintCallable, Category = "Logistics")
	void UpdateInventoryOfferings(USAS_InventoryComponent* SourceInventory, const TArray<FSAS_LogisticsOffering>& NewOfferings);

	UFUNCTION(BlueprintCallable, Category = "Logistics")
	TArray<USAS_WorkerControlComponent*> GetAvailableWorkers() const;

	//Dispatchers

	UPROPERTY(BlueprintAssignable, Category = "Logistics")
	FNotifyLogisticsMasterJobUpdated NotifyLogisticsMasterJobUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Logistics")
	FNotifyGroupedLogisticsOfferingsUpdated NotifyGroupedLogisticsOfferingsUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Logistics")
	FNotifyAvailableWorkersUpdated NotifyAvailableWorkersUpdated;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Logistics")
	void GetOfferingsGroupedBySource(TArray<FSAS_LogisticsOfferingGroup>& OutGroups) const;

private:
	void HandleLogisticsMasterJobUpdated(USAS_LogisticsMasterJob* LogisticsMasterJob);

	void TryAssignJobs();

	bool FindBestTransportCandidate(USAS_LMJ_DeliverItem* DeliveryJob, const TArray<TObjectPtr<USAS_WorkerControlComponent>>& WorkersToConsider, USAS_WorkerControlComponent*& OutBestWorker, FSAS_LogisticsOffering& OutBestOffering, int32& OutAssignedAmount);

	bool GetCachedOrComputePathLength(AActor* SourceActor, AActor* TargetActor, float& OutPathLength);

public:	

	

protected:

	UPROPERTY()
	TArray<TObjectPtr<USAS_LogisticsMasterJob>> ActiveJobs;

	UPROPERTY()
	TArray<TObjectPtr<USAS_WorkerControlComponent>> AvailableWorkers;

	UPROPERTY()
	TArray<FSAS_LogisticsOffering> ActiveOfferings;

	UPROPERTY()
	TMap<FSAS_SourceTargetPair, FSAS_PathDistanceCache> CachedPathDistances;

};
