// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_InventoryProfileData.generated.h"

/**
 * 
 */

class UItemDefinitionPrimaryData;

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_InventoryProfileData : public UDataAsset
{
	GENERATED_BODY()
	


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 NumSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 DefaultMaxPerSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TMap<UItemDefinitionPrimaryData*, int32> MaxPerSlotOverrides;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	bool bCanOnlyStoreOneResource = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	UItemDefinitionPrimaryData* OnlyStorableResource = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	ESAS_TrackedInventoryCategory TrackedInventoryCategory = ESAS_TrackedInventoryCategory::None;

};
