

#pragma once

#include "CoreMinimal.h"
#include "Misc/Structs/SAS_LogisticsOfferingEntry.h"
#include "SAS_LogisticsOfferingGrouped.generated.h"

class UItemDefinitionPrimaryData;
class USAS_InventoryComponent;

USTRUCT(BlueprintType)
struct FSAS_LogisticsOfferingGroup
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USAS_InventoryComponent> SourceInventory = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FSAS_LogisticsOfferingEntry> OfferedItems;
};