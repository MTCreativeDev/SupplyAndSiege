#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "Engine/AssetManagerTypes.h"
#include "SAS_InventoryManagerComponent.generated.h"

class USAS_InventoryComponent;
class UItemDefinitionPrimaryData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnTeamInventoryChanged,
	ESAS_Team, Team,
	FPrimaryAssetId, ItemId,
	int32, Delta,
	int32, NewTotal
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USAS_InventoryManagerComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RegisterTeamInventory(USAS_InventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UnregisterTeamInventory(USAS_InventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GetInventoryTotals(ESAS_TrackedInventoryCategory InventoryToGet, TMap<FPrimaryAssetId, int32>& OutTotals) const;

	//Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTeamInventoryChanged OnTeamStockpileInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTeamInventoryChanged OnTeamTransitInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTeamInventoryChanged OnTeamRefinerInputInventoryChanged;



protected:

	virtual void BeginPlay() override;



private:
	
	UFUNCTION()
	void HandleInventoryChanged(USAS_InventoryComponent* Sender, FPrimaryAssetId ItemId, int32 Delta);

	void AddEntireInventoryToTrackedInventory(ESAS_TrackedInventoryCategory TrackedInventory, USAS_InventoryComponent* Inventory);

	void RemoveEntireInventoryFromTrackedInventory(ESAS_TrackedInventoryCategory TrackedInventory, USAS_InventoryComponent* Inventory);

	void ApplyDeltaAndBroadcast(ESAS_TrackedInventoryCategory InventoryToModify, const FPrimaryAssetId& ItemId, int32 Delta);

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	ESAS_Team AssignedTeam = ESAS_Team::None;

private:
	UPROPERTY()
	TMap<FPrimaryAssetId, int32> TeamStockpileTotals;

	UPROPERTY()
	TMap<FPrimaryAssetId, int32> TeamTransitTotals;

	UPROPERTY()
	TMap<FPrimaryAssetId, int32> TeamRefinerInputTotals;

	UPROPERTY()
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> StockpileInventories;

	UPROPERTY()
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> TransitInventories;

	UPROPERTY()
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> RefinerInputInventories;

	UPROPERTY()
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> AllTeamUnitInventories;

		
};
