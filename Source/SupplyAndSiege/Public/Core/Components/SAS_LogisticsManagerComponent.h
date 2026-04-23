

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "Misc/Structs/SAS_LogisticsOffering.h"
#include "SAS_LogisticsManagerComponent.generated.h"

class USAS_LogisticsMasterJob;
class USAS_WorkerControlComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyLogisticsMasterJobUpdated, USAS_LogisticsMasterJob*, LogisticsMasterJob);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_LogisticsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_LogisticsManagerComponent();

	void CreateBuildSiteResourceDeliveryJob(const FSAS_ResourceDeliveryRequest& ResourceDeliveryInformation);

	UFUNCTION(BlueprintCallable, Category = "Logistics")
	TArray<USAS_LogisticsMasterJob*> GetActiveJobs() const { return ActiveJobs; }

	void RegisterAvailableWorker(USAS_WorkerControlComponent* Worker);
	void UnregisterAvailableWorker(USAS_WorkerControlComponent* Worker);

	//Dispatchers

	UPROPERTY(BlueprintAssignable, Category = "Logistics")
	FNotifyLogisticsMasterJobUpdated NotifyLogisticsMasterJobUpdated;

	UFUNCTION(BlueprintCallable, Category = "Logistics")
	void UpdateInventoryOfferings(USAS_InventoryComponent* SourceInventory, const TArray<FSAS_LogisticsOffering>& NewOfferings);

protected:
	virtual void BeginPlay() override;

private:
	void HandleLogisticsMasterJobUpdated(USAS_LogisticsMasterJob* LogisticsMasterJob);

	void TryAssignJobs();

public:	

	

protected:

	TArray<TObjectPtr<USAS_LogisticsMasterJob>> ActiveJobs;
	TArray<TObjectPtr<USAS_WorkerControlComponent>> AvailableWorkers;

	UPROPERTY()
	TArray<FSAS_LogisticsOffering> ActiveOfferings;

};
