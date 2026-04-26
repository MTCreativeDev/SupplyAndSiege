// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_UnitTypeData.generated.h"


class UTexture2D;
class USAS_HealthDefinitionData;
class USAS_VisionDefinitionData;
/**
 * 
 */
UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_UnitTypeData : public UDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UnitId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESAS_UnitCategory UnitCategory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USAS_HealthDefinitionData* HealthDefinition = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USAS_VisionDefinitionData* VisionDefinition = nullptr;

};
