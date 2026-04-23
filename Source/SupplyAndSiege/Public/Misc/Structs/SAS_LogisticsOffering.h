

#pragma once

#include "CoreMinimal.h"
#include "SAS_LogisticsOffering.generated.h"

class USAS_InventoryComponent;
class UItemDefinitionPrimaryData;

USTRUCT(BlueprintType)
struct FSAS_LogisticsOffering
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics")
	TObjectPtr<USAS_InventoryComponent> SourceInventory = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics")
	TObjectPtr<UItemDefinitionPrimaryData> Item = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics")
	int32 Quantity = 0;
};