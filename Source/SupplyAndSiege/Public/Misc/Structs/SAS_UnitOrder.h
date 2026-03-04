// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/SAS_Enumerators.h"
#include "Misc/DataAssets/SAS_ResourceTypeData.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "SAS_UnitOrder.generated.h"


USTRUCT(BlueprintType)
struct SUPPLYANDSIEGE_API FSAS_UnitOrder
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) ESAS_UnitOrderType Type = ESAS_UnitOrderType::None;
	UPROPERTY(BlueprintReadOnly) TObjectPtr<USAS_ResourceTypeData> ResourceType = nullptr;
	UPROPERTY(BlueprintReadOnly) FSAS_ResourceKey ResourceKey;
	UPROPERTY(BlueprintReadOnly) FVector TargetLocation = FVector::ZeroVector;

};