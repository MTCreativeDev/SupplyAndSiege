// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_InventoryManagerComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"


USAS_InventoryManagerComponent::USAS_InventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_InventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USAS_InventoryManagerComponent::RegisterTeamInventory(USAS_InventoryComponent* Inventory)
{
	if (!Inventory) return;
	if (Inventory->GetAssignedTeam() != AssignedTeam) return;
	if (AllTeamUnitInventories.Contains(Inventory)) return;

	AllTeamUnitInventories.Add(Inventory);

	const USAS_InventoryProfileData* Profile = Inventory->GetInventoryProfile();
	const bool bIsStockpile = Profile && Profile->bIsStockpileInventory;

	if (bIsStockpile && !StockpileInventories.Contains(Inventory))
	{
		StockpileInventories.Add(Inventory);
		AddEntireInventoryToStockpile(Inventory);
	}

	Inventory->OnInventoryChanged.RemoveDynamic(this, &USAS_InventoryManagerComponent::HandleInventoryChanged);
	Inventory->OnInventoryChanged.AddDynamic(this, &USAS_InventoryManagerComponent::HandleInventoryChanged);

}

void USAS_InventoryManagerComponent::UnregisterTeamInventory(USAS_InventoryComponent* Inventory)
{
	if (!Inventory) return;

	//In off case this is trying to de-register when it was never registered
	if (!AllTeamUnitInventories.Contains(Inventory)) return;

	Inventory->OnInventoryChanged.RemoveDynamic(this, &USAS_InventoryManagerComponent::HandleInventoryChanged);

	if (StockpileInventories.Contains(Inventory))
	{
		RemoveEntireInventoryFromStockpile(Inventory);
		StockpileInventories.Remove(Inventory);
	}

	AllTeamUnitInventories.Remove(Inventory);
}

void USAS_InventoryManagerComponent::GetAllStockpuleTotals(TMap<FPrimaryAssetId, int32>& OutTotals) const
{
	OutTotals = TeamStockpileTotals;
}

void USAS_InventoryManagerComponent::HandleInventoryChanged(USAS_InventoryComponent* Sender, FPrimaryAssetId ItemId, int32 Delta)
{
	if (!Sender) return;
	if (!ItemId.IsValid()) return;
	if (Delta == 0) return;
	if (!AllTeamUnitInventories.Contains(Sender)) return;
	if (!StockpileInventories.Contains(Sender)) return;

	ApplyDeltaAndBroadcast(ItemId, Delta);

}

void USAS_InventoryManagerComponent::AddEntireInventoryToStockpile(USAS_InventoryComponent* Inventory)
{
	if (!Inventory) return;

	TMap<FPrimaryAssetId, int32> Snapshot;
	Inventory->GetAllItemTotals(Snapshot);

	//Key Value Pair
	for (const TPair<FPrimaryAssetId, int32>& KVP : Snapshot)
	{
		const FPrimaryAssetId& ItemId = KVP.Key;
		const int32 Amount = KVP.Value;

		if (!ItemId.IsValid()) continue;
		if (Amount == 0) continue;

		ApplyDeltaAndBroadcast(ItemId, Amount);
	}
}

void USAS_InventoryManagerComponent::RemoveEntireInventoryFromStockpile(USAS_InventoryComponent* Inventory)
{
	if (!Inventory) return;

	TMap<FPrimaryAssetId, int32> Snapshot;
	Inventory->GetAllItemTotals(Snapshot);

	//Key Value Pair
	for (const TPair<FPrimaryAssetId, int32>& KVP : Snapshot)
	{
		const FPrimaryAssetId& ItemId = KVP.Key;
		const int32 Amount = KVP.Value;

		if (!ItemId.IsValid()) continue;
		if (Amount == 0) continue;

		ApplyDeltaAndBroadcast(ItemId, -Amount);
	}
}

void USAS_InventoryManagerComponent::ApplyDeltaAndBroadcast(const FPrimaryAssetId& ItemId, int32 Delta)
{
	int32& Total = TeamStockpileTotals.FindOrAdd(ItemId);
	Total += Delta;

	if (Total <= 0)
	{
		TeamStockpileTotals.Remove(ItemId);
		Total = 0;
	}

	OnTeamInventoryChanged.Broadcast(AssignedTeam, ItemId, Delta, Total);

}

