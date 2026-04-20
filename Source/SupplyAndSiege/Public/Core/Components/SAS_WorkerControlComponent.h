

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "SAS_WorkerControlComponent.generated.h"

class USAS_LogisticsManagerComponent;
class USAS_LogisticsWorkerAssignment;
class USAS_UnitInformationComponent;
class USAS_ResourceTypeData;
struct FGameplayTag;


UENUM(BlueprintType)
enum class ESAS_WorkerControlState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	ManualPersistent	UMETA(DisplayName = "Manual Persistant"),
	ManualSingle	UMETA(DisplayName = "Manual Single"),
	Transition	UMETA(DisplayName = "Transition"),
	LMQueue	UMETA(DisplayName = "Logistics Manager Queue")
};

UENUM(BlueprintType)
enum class ESAS_WorkerRequestResult :uint8
{
	Rejected	UMETA(DisplayName = "Rejected"),
	Accepted	UMETA(DisplayName = "Accepted"),
	StartedTransition	UMETA(DisplayName = "Started Transition")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_WorkerControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_WorkerControlComponent();

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	virtual ESAS_WorkerRequestResult RequestManualMove(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	virtual ESAS_WorkerRequestResult RequestManualHarvest(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation);

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	virtual ESAS_WorkerRequestResult RequestEnterLmQueue();

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	virtual bool TryAcceptAssignment(USAS_LogisticsWorkerAssignment* NewAssignment);

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	virtual void NotifyAssignmentFinished(USAS_LogisticsWorkerAssignment* FinishedAssignment);

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	virtual void NotifyAssignmentCancelled(USAS_LogisticsWorkerAssignment* CancelledAssignment);

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	ESAS_WorkerControlState GetCurrentWorkerControlState() const { return CurrentWorkerControlState; }

	UFUNCTION(BlueprintCallable, Category = "Worker Control")
	bool UpdateCurrentHarvestKeyAndLocation(FSAS_ResourceKey ResourceKey, FVector TargetLocation);

protected:

	virtual void BeginPlay() override;

	virtual bool ExitLmControlForPlayerOverride();
	virtual bool CanEnterLmQueueImmediately() const;
	virtual void StartTransitionIntoLmQueue();
	virtual void FinishEnterLmQueue();

	virtual void BeginManualMove_Internal(const FVector& WorldLocation);
	virtual void BeginManualHarvest_Internal(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation);

	void SendStateTreeEvent(const FGameplayTag& EventTag);


public:	

protected:
	UPROPERTY()
	TObjectPtr<USAS_UnitInformationComponent> UnitInformation = nullptr;

	UPROPERTY()
	TObjectPtr<USAS_LogisticsManagerComponent> LogisticsManager = nullptr;

	UPROPERTY()
	TObjectPtr<USAS_LogisticsWorkerAssignment> ActiveAssignment = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker Control")
	ESAS_WorkerControlState CurrentWorkerControlState = ESAS_WorkerControlState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker Control")
	USAS_ResourceTypeData* CurrentHarvestResourceType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker Control")
	FSAS_ResourceKey CurrentHarvestResourceKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Worker Control")
	FVector CurrentHarvestTargetLocation = FVector::ZeroVector;

		
};
