

#pragma once

#include "CoreMinimal.h"
#include "Core/SAS_Enumerators.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "SAS_WA_FailureContext.generated.h"

USTRUCT(BlueprintType)
struct SUPPLYANDSIEGE_API FSAS_WA_FailureContext
{

	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ESAS_WorkerAssignmentFailureReason Reason = ESAS_WorkerAssignmentFailureReason::None;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<USAS_UnitInformationComponent> Worker = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bRejectWorkerForThisMasterJob = false;

	UPROPERTY(BlueprintReadOnly)
	bool bReEvaluateMasterJobValidity = false;

	UPROPERTY(BlueprintReadOnly)
	bool bEditorDebug = false;

	UPROPERTY(BlueprintReadOnly)
	FText FailureDescription;

};
