

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_LogisticsMasterJob.generated.h"

class USAS_LogisticsManagerComponent;
class USAS_LogisticsWorkerAssignment;
struct FSAS_WA_FailureContext;

UCLASS()
class SUPPLYANDSIEGE_API USAS_LogisticsMasterJob : public UObject
{
	GENERATED_BODY()
	

public:
	void InitializeJob(USAS_LogisticsManagerComponent* InOwningLM, ESAS_MasterJobType InJobType, int32 InPriority, AActor* InRequestingActor);

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void AddAssignment(USAS_LogisticsWorkerAssignment* Assignment);
	
	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void NotifyAssignmentCompleted(USAS_LogisticsWorkerAssignment* Assignment);

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* Assignment);

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void NotifyAssignmentFailed(USAS_LogisticsWorkerAssignment* Assignment, FSAS_WA_FailureContext FailureContext);

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void MarkCancelled();

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	bool IsFinished() const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void RemoveAssignment(USAS_LogisticsWorkerAssignment* Assignment);

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual bool EvaluateIsComplete() const;

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void MarkCompleted();

	UFUNCTION(BlueprintCallable, Category = "Logistics Master Job")
	virtual void MarkFailed();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Master Job")
	FGuid MasterJobID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Master Job")
	ESAS_MasterJobType MasterJobType = ESAS_MasterJobType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Master Job")
	ESAS_MasterJobStatus MasterJobStatus = ESAS_MasterJobStatus::Pending;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Master Job")
	int32 Priority = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Master Job")
	TObjectPtr<AActor> RequestingActor = nullptr;

	//I dont think this will be needed, but just in case the AI ends up with its own LM (Ideal, but likely out of scope)
	UPROPERTY()
	TObjectPtr<USAS_LogisticsManagerComponent> OwningLM = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Master Job")
	TArray <TObjectPtr<USAS_LogisticsWorkerAssignment>> ActiveAssignments;
};
