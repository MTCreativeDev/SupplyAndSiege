

#pragma once

#include "CoreMinimal.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/Structs/SAS_InventoryPriorityThreshold.h"
#include "Misc/Structs/SAS_LogisticsOffering.h"
#include "SAS_IC_ResourceDepot.generated.h"

class UItemDefinitionPrimaryData;
class USAS_LogisticsManagerComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_IC_ResourceDepot : public USAS_InventoryComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory|Logistics")
	void GetOfferedItems(TMap<UItemDefinitionPrimaryData*, int32>& OutOffers) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Logistics")
	void GetRequestedRemovals(TMap<UItemDefinitionPrimaryData*, int32>& OutRequests) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Logistics")
	int32 GetRemovalPriorityForQuantity(int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Logistics")
	bool ShouldBroadcastOffers() const { return bBroadcastOffers; }

	UFUNCTION(BlueprintCallable, Category = "Inventory|Logistics")
	bool ShouldBroadcastRemovalRequests() const { return bRequestRemovalWhenNotEmpty; }

	virtual int32 AddItem(UItemDefinitionPrimaryData* Item, int32 Quantity) override;

	virtual int32 RemoveItem(UItemDefinitionPrimaryData* Item, int32 Quantity) override;



protected:
	virtual void BeginPlay() override;

	void SortPriorityThresholds();


private:

	void RefreshLogisticsState();

	void GetCurrentOfferings(TArray<FSAS_LogisticsOffering>& OutOfferings) const;


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Logistics")
	bool bBroadcastOffers = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Logistics")
	bool bRequestRemovalWhenNotEmpty = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Logistics")
	TArray<FSAS_InventoryPriorityThreshold> RemovalPriorityThresholds;

	USAS_LogisticsManagerComponent* LogisticsManager = nullptr;




};
