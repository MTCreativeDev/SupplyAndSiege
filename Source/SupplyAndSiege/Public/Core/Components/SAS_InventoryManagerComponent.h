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

	//Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTeamInventoryChanged OnTeamInventoryChanged;



protected:

	virtual void BeginPlay() override;

private:
	
	UFUNCTION()
	void HandleInventoryChanged(USAS_InventoryComponent* Sender, FPrimaryAssetId ItemId, int32 Delta);

	void AddEntireInventoryToStockpile(USAS_InventoryComponent* Inventory);

	void RemoveEntireInventoryFromStockpile(USAS_InventoryComponent* Inventory);

	void ApplyDeltaAndBroadcast(const FPrimaryAssetId& ItemId, int32 Delta);

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	ESAS_Team AssignedTeam = ESAS_Team::None;

private:
	UPROPERTY()
	TMap<FPrimaryAssetId, int32> TeamStockpileTotals;

	//The inventories that will contribute to the TeamStockpileTotals. We ignore villagers transfering materials etc to avoid constant updates.
	UPROPERTY()
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> StockpileInventories;

	UPROPERTY()
	TSet<TWeakObjectPtr<USAS_InventoryComponent>> AllTeamUnitInventories;

		
};
