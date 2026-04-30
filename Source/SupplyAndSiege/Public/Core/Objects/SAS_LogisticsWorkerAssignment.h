

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Core/Sas_Enumerators.h"
#include "SAS_LogisticsWorkerAssignment.generated.h"

class AActor;
class UItemDefinitionPrimaryData;
class USAS_LogisticsMasterJob;
class USAS_WorkerControlComponent;
struct FSAS_WA_FailureContext;

UCLASS()
class SUPPLYANDSIEGE_API USAS_LogisticsWorkerAssignment : public UObject
{
	GENERATED_BODY()

public:
	void InitializeAssignment(USAS_LogisticsMasterJob* InParentMasterJob, USAS_WorkerControlComponent* InAssignedWorker, ESAS_WorkerAssignmentType InAssignmentType);

	virtual void StartAssignment();
	virtual void CancelAssignment();
	virtual void FailAssignment(FSAS_WA_FailureContext FailureContext);
	virtual void CompleteAssignment();

	UFUNCTION(BlueprintPure, Category = "Logistics Worker Assignment")
	bool IsFinished() const;

	UFUNCTION(BlueprintPure, Category = "Logistics Worker Assignment")
	virtual int32 GetAssignedAmount() const { return 0; }

	UFUNCTION(BlueprintPure, Category = "Logistics Worker Assignment")
	virtual UItemDefinitionPrimaryData* GetItemDefinition() const { return nullptr; }

	UFUNCTION(BlueprintPure, Category = "Logistics Worker Assignment")
	virtual AActor* GetSourceActor() const { return nullptr; }

	UFUNCTION(BlueprintPure, Category = "Logistics Worker Assignment")
	virtual AActor* GetTargetActor() const { return nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Logistics Worker Assignment")
	virtual void NotifyWorkerActionAccepted();

	UFUNCTION(BlueprintCallable, Category = "Logistics Worker Assignment")
	virtual void NotifyWorkerActionFailed(ESAS_WorkerAssignmentFailureReason Reason);

	UFUNCTION(BlueprintCallable, Category = "Logistics Worker Assignment")
	virtual void NotifyWorkerActionCompleted();

	UFUNCTION(BlueprintCallable, Category = "Logistics Worker Assignment")
	virtual void NotifyWorkerActionInterrupted(ESAS_WorkerAssignmentFailureReason Reason);

protected:
	virtual void NotifyParentAssignmentCompleted();
	virtual void NotifyParentAssignmentCancelled();
	virtual void NotifyParentAssignmentFailed(FSAS_WA_FailureContext FailureContext);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Worker Assignment")
	FGuid WorkerAssignmentID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Worker Assignment")
	ESAS_WorkerAssignmentType WorkerAssignmentType = ESAS_WorkerAssignmentType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Worker Assignment")
	ESAS_WorkerAssignmentStatus WorkerAssignmentStatus = ESAS_WorkerAssignmentStatus::Pending;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Worker Assignment")
	TObjectPtr<USAS_LogisticsMasterJob> ParentMasterJob = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics Worker Assignment")
	TObjectPtr<USAS_WorkerControlComponent> AssignedWorker = nullptr;
	
	
};
