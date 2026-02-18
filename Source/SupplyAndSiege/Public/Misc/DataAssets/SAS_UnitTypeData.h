// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_UnitTypeData.generated.h"


class UTexture2D;
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
	UTexture2D* Icon = nullptr;

};
