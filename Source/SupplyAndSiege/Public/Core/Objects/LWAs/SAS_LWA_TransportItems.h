

#pragma once

#include "CoreMinimal.h"
#include "Core/Objects/SAS_LogisticsWorkerAssignment.h"
#include "Misc/Structs/SAS_InventoryReservationHandle.h"
#include "SAS_LWA_TransportItems.generated.h"

class UItemDefinitionPrimaryData;
class USAS_LogisticsManagerComponent;
class USAS_InventoryComponent;

UENUM(BlueprintType)
enum class ESAS_TransportAssignmentPhase : uint8
{
	None UMETA(DisplayName = "None"),
	Entry UMETA(DisplayName = "Entry"),
	MovingToSource UMETA(DisplayName = "Moving to Source"),
	PickingUp UMETA(DisplayName = "Picking Up"),
	MovingToTarget UMETA(DisplayName = "Moving to Target"),
	DroppingOff UMETA(DisplayName = "Dropping Off")
};


UCLASS()
class SUPPLYANDSIEGE_API USAS_LWA_TransportItems : public USAS_LogisticsWorkerAssignment
{
	GENERATED_BODY()
public:
	
	void InitializeTransportAssignment(USAS_LogisticsMasterJob* InParentMasterJob, USAS_WorkerControlComponent* InAssignedWorker, UItemDefinitionPrimaryData* InItemDefinition, int32 InAssignedAmount, AActor* InSourceActor, AActor* InTargetActor);

	virtual void StartAssignment() override;
	virtual void CancelAssignment() override;
	virtual void FailAssignment(FSAS_WA_FailureContext FailureContext) override;
	virtual void CompleteAssignment() override;


	virtual int32 GetAssignedAmount() const override { return AssignedAmount; }
	virtual UItemDefinitionPrimaryData* GetItemDefinition() const override { return ItemDefinition; }
	virtual AActor* GetSourceActor() const override { return SourceActor; }
	virtual AActor* GetTargetActor() const override { return TargetActor; }

	virtual void NotifyWorkerActionAccepted() override;
	virtual void NotifyWorkerActionFailed(ESAS_WorkerAssignmentFailureReason Reason) override;
	virtual void NotifyWorkerActionCompleted() override;
	virtual void NotifyWorkerActionInterrupted(ESAS_WorkerAssignmentFailureReason Reason) override;

protected:
	bool RunEntryChecks();
	void RequestMoveToSource();
	bool ReserveOutboundInventory();
	bool ReserveInboundInventory();
	void ReleaseReservations();
	bool PickupReservation();
	void RequestMoveToTarget();
	bool DropoffReservation();


private:


	FSAS_WA_FailureContext MakeFailure(ESAS_WorkerAssignmentFailureReason Reason, bool EditorDebug = true, bool RejectWorkerForThisMasterJob = false, bool ReEvaluateMasterJobValidity = false) const;



protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	ESAS_TransportAssignmentPhase CurrentPhase = ESAS_TransportAssignmentPhase::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	TObjectPtr<UItemDefinitionPrimaryData> ItemDefinition = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	int32 AssignedAmount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	FSAS_InventoryReservationHandle OutboundReservationHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	FSAS_InventoryReservationHandle InboundReservationHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transport Assignment")
	bool bPickedUpReservation = false;

};
