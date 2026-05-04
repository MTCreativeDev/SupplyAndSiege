

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_SelectedUnitWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API USAS_SelectedUnitWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Selection Inventory")
	void HandleSelectionInventoryTotalsChanged(const TMap<FPrimaryAssetId, int32>& NewInventoryTotals);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ExposeOnSpawn = "true" ))
	TMap<FPrimaryAssetId, int32> InventoryTotals;

	
};
