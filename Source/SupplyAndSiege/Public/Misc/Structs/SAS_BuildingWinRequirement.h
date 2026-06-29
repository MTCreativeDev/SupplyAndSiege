

#pragma once

#include "CoreMinimal.h"
#include "SAS_BuildingWinRequirement.generated.h"

class ASAS_BuildingLayout;

USTRUCT(BlueprintType)
struct FSAS_BuildingWinRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ASAS_BuildingLayout> BuildingClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredAmount = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentAmount = 0;
};
