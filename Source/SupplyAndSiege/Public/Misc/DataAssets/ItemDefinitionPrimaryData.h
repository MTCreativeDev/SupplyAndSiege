// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDefinitionPrimaryData.generated.h"

/**
 * 
 */
class UTexture2D;

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API UItemDefinitionPrimaryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

};
