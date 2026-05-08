// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAS_GameDataAsset.h"
#include "ItemDefinitionPrimaryData.generated.h"

/**
 * 
 */
class UTexture2D;

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API UItemDefinitionPrimaryData : public USAS_GameDataAsset
{
	GENERATED_BODY()

	UItemDefinitionPrimaryData()
	{
		CanBeProduced = true;
	}

};
