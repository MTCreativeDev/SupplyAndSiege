// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAS_GameDataAsset.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_UnitTypeData.generated.h"


class UTexture2D;
class USAS_HealthDefinitionData;
/**
 * 
 */
UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_UnitTypeData : public USAS_GameDataAsset
{
	GENERATED_BODY()
	

	USAS_UnitTypeData()
	{
		CanBeProduced = true;
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UnitId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESAS_UnitCategory UnitCategory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USAS_HealthDefinitionData* HealthDefinition = nullptr;

};
