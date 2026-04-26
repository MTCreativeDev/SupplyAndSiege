// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_VisionDefinitionData.generated.h"

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_VisionDefinitionData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Vision radius in centimeters. Must be > 0.0 to grant vision. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vision")
	float Radius = 0.0f;
};
