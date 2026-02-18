// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAS_InventorySlot.generated.h"

class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct FSAS_InventorySlot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItemDefinitionPrimaryData* Item = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 RemainingSpaceInSlot = 1;

	bool IsEmpty() const { return Item == nullptr || Quantity <= 0; }
	void ClearSlot() { Item = nullptr; Quantity = 0; }
};