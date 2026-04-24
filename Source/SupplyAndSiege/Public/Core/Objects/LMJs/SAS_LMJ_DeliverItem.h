

#pragma once

#include "CoreMinimal.h"
#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "SAS_LMJ_DeliverItem.generated.h"

class UItemDefinitionPrimaryData;
class USAS_LogisticsWorkerAssignment;


UCLASS()
class SUPPLYANDSIEGE_API USAS_LMJ_DeliverItem : public USAS_LogisticsMasterJob
{
	GENERATED_BODY()
	

public:

	void InitializeDeliverItemJob(USAS_LogisticsManagerComponent* InOwningLM, AActor* InRequestingActor, UItemDefinitionPrimaryData* InItemDefinition, int32 InRequestedAmount, int32 InPriority);

	virtual void AddAssignment(USAS_LogisticsWorkerAssignment* Assignment) override;
	virtual void NotifyAssignmentCompleted(USAS_LogisticsWorkerAssignment* Assignment) override;
	virtual void NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* Assignment) override;
	virtual void NotifyAssignmentFailed(USAS_LogisticsWorkerAssignment* Assignment, FSAS_WA_FailureContext FailureContext) override;

	virtual FSAS_LogisticsJobWidgetInfo GetJobInfoForWidget() const override;

	UFUNCTION(BlueprintPure, Category = "Logistics Master Job")
	int32 GetRemainingAmount() const;

	UFUNCTION(BlueprintPure, Category = "Logistics Master Job")
	int32 GetUnreservedAmount() const;

	UFUNCTION(BlueprintPure, Category = "Logistics Master Job")
	bool CanAcceptMoreAssignments() const;

	TObjectPtr< UItemDefinitionPrimaryData> GetItemDefinition() const { return ItemDefinition; }

protected:
	virtual bool EvaluateIsComplete() const override;



protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logisticsc Master Job")
	TObjectPtr<UItemDefinitionPrimaryData> ItemDefinition = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logisticsc Master Job")
	int32 RequestedAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logisticsc Master Job")
	int32 DeliveredAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logisticsc Master Job")
	int32 ReservedAmount = 0;


};
