#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/AssetManagerTypes.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_SelectionInventoryViewModel.generated.h"

class USAS_UnitManagerComponent;
class USAS_UnitInformationComponent;
class USAS_InventoryComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionInventoryModelChanged);

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_SelectionInventoryViewModel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Selection Inventory")
	void Initialize(USAS_UnitManagerComponent* InUnitManager);

	UFUNCTION(BlueprintCallable, Category = "Selection Inventory")
	void Shutdown();

	UFUNCTION(BlueprintCallable, Category = "Selection Inventory")
	ESAS_SelectionInventoryMode GetMode() const { return Mode;}

	UFUNCTION(BlueprintCallable, Category = "Selection Inventory")
	const TMap<FPrimaryAssetId, int32>& GetGroupTotals() const { return GroupTotals; }

	//Dispatchers

	UPROPERTY(BlueprintAssignable, Category = "Selection Inventory")
	FOnSelectionInventoryModelChanged OnSelectionInventoryModelChanged;

private:
	UFUNCTION()
	void HandleSelectedUnitsChanged(const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>& NewSelection);

	void RebuildObservedInventories();
	void BindObservedInventories();
	void UnbindAllObservedInventories();


	UFUNCTION()
	void HandleObservedInventoryChanged(USAS_InventoryComponent* Sender, FPrimaryAssetId ItemID, int32 Delta);

	void RebuildModel();
	void RebuildGroupTotals_AllInventories();




private:
	UPROPERTY(Transient)
	TObjectPtr<USAS_UnitManagerComponent> UnitManager;
	
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<USAS_UnitInformationComponent>> SelectedUnits;

	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> ObservedInventories;

	UPROPERTY(Transient)
	ESAS_SelectionInventoryMode Mode = ESAS_SelectionInventoryMode::None;

	UPROPERTY(Transient)
	TMap<FPrimaryAssetId, int32> GroupTotals;

};
