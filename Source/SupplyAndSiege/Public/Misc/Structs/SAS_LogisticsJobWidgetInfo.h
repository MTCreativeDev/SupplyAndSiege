

#pragma once

#include "CoreMinimal.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_LogisticsJobWidgetInfo.generated.h"

class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct SUPPLYANDSIEGE_API FSAS_LogisticsJobWidgetInfo
{
GENERATED_BODY()

UPROPERTY(BlueprintReadOnly)
ESAS_MasterJobType JobType = ESAS_MasterJobType::None;

UPROPERTY(BlueprintReadOnly)
TObjectPtr<AActor> RequestingActor = nullptr;

UPROPERTY(BlueprintReadOnly)
ESAS_MasterJobStatus JobStatus = ESAS_MasterJobStatus::Pending;

//Deliver Item Specific
UPROPERTY(BlueprintReadOnly)
int32 RequestedAmount = 0;

UPROPERTY(BlueprintReadOnly)
int32 ReservedAmount = 0;

UPROPERTY(BlueprintReadOnly)
int32 DeliveredAmount = 0;

UPROPERTY(BlueprintReadOnly)
TObjectPtr<UItemDefinitionPrimaryData> ItemDefinition = nullptr;


};
