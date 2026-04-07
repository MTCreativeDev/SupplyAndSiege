// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_InventoryManagerComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"


USAS_InventoryManagerComponent::USAS_InventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_InventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

FSAS_ItemInventoryPerCategory USAS_InventoryManagerComponent::GetItemInventoryPerCategory(UItemDefinitionPrimaryData* ItemDefinition) const
{
	FSAS_ItemInventoryPerCategory Result;

	if (!ItemDefinition) return Result;

	const FPrimaryAssetId ItemId = ItemDefinition->GetPrimaryAssetId();
	if (!ItemId.IsValid()) return Result;

	if (const int32* Found = TeamStockpileTotals.Find(ItemId)) Result.StockpileAmount = *Found;

	if (const int32* Found = TeamTransitTotals.Find(ItemId)) Result.TransitAmount = *Found;

	if (const int32* Found = TeamRefinerInputTotals.Find(ItemId)) Result.RefinerInputAmount = *Found;
	
	Result.Total = Result.StockpileAmount + Result.TransitAmount + Result.RefinerInputAmount;

	return Result;
}

void USAS_InventoryManagerComponent::RegisterTeamInventory(USAS_InventoryComponent* Inventory)
{
	if (!Inventory) return;
	if (Inventory->GetAssignedTeam() != AssignedTeam) return;
	if (AllTeamUnitInventories.Contains(Inventory)) return;

	AllTeamUnitInventories.Add(Inventory);

	const USAS_InventoryProfileData* Profile = Inventory->GetInventoryProfile();
	if (!Profile) return;

	const ESAS_TrackedInventoryCategory TrackedInventoryCategory = Profile->TrackedInventoryCategory;
	TSet<TWeakObjectPtr<USAS_InventoryComponent>>* TargetSet = nullptr;

	switch (TrackedInventoryCategory)
	{
		case ESAS_TrackedInventoryCategory::Stockpile:
			TargetSet = &StockpileInventories;
			break;
		case ESAS_TrackedInventoryCategory::Transit:
			TargetSet = &TransitInventories;
			break;
		case ESAS_TrackedInventoryCategory::RefinerInput:
			TargetSet = &RefinerInputInventories;
			break;
		default: break;
	}

	if (TargetSet && !TargetSet->Contains(Inventory))
	{
		TargetSet->Add(Inventory);
		AddEntireInventoryToTrackedInventory(TrackedInventoryCategory, Inventory);
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
	AllTeamUnitInventories.Remove(Inventory);

	const USAS_InventoryProfileData* Profile = Inventory->GetInventoryProfile();
	if (!Profile) return;
	const ESAS_TrackedInventoryCategory TrackedInventory = Profile->TrackedInventoryCategory;
	
	TSet<TWeakObjectPtr<USAS_InventoryComponent>>* TargetSet = nullptr;

	switch (TrackedInventory)
	{
	case ESAS_TrackedInventoryCategory::Stockpile:
		TargetSet = &StockpileInventories;
		break;
	case ESAS_TrackedInventoryCategory::Transit:
		TargetSet = &TransitInventories;
		break;
	case ESAS_TrackedInventoryCategory::RefinerInput:
		TargetSet = &RefinerInputInventories;
		break;
	default: return;
	}
	if (TargetSet->Remove(Inventory) > 0)
	{
		RemoveEntireInventoryFromTrackedInventory(TrackedInventory, Inventory);
	}
}

void USAS_InventoryManagerComponent::GetInventoryTotals(ESAS_TrackedInventoryCategory InventoryToGet, TMap<FPrimaryAssetId, int32>& OutTotals) const
{
	OutTotals.Reset();
	switch (InventoryToGet)
	{
	case ESAS_TrackedInventoryCategory::Stockpile:
		OutTotals = TeamStockpileTotals;
		break;
	case ESAS_TrackedInventoryCategory::Transit:
		OutTotals = TeamTransitTotals;
		break;
	case ESAS_TrackedInventoryCategory::RefinerInput:
		OutTotals = TeamRefinerInputTotals;
		break;
	default: break;
	}
}

void USAS_InventoryManagerComponent::HandleInventoryChanged(USAS_InventoryComponent* Sender, FPrimaryAssetId ItemId, int32 Delta)
{
	if (!Sender) return;
	if (!ItemId.IsValid()) return;
	if (Delta == 0) return;
	if (!AllTeamUnitInventories.Contains(Sender)) return;

	const USAS_InventoryProfileData* InventoryProfile = Sender->GetInventoryProfile();
	if (!InventoryProfile) return;

	const ESAS_TrackedInventoryCategory TrackedInventory = InventoryProfile->TrackedInventoryCategory;

	switch(TrackedInventory)
	{
	case ESAS_TrackedInventoryCategory::Stockpile:
		if (!StockpileInventories.Contains(Sender)) return;
		break;
	case ESAS_TrackedInventoryCategory::Transit:
		if (!TransitInventories.Contains(Sender)) return;
		break;
	case ESAS_TrackedInventoryCategory::RefinerInput:
		if (!RefinerInputInventories.Contains(Sender)) return;
		break;
	default: return;
	}

	ApplyDeltaAndBroadcast(TrackedInventory, ItemId, Delta);
}

void USAS_InventoryManagerComponent::AddEntireInventoryToTrackedInventory(ESAS_TrackedInventoryCategory TrackedInventory, USAS_InventoryComponent* Inventory)
{
	if (!Inventory || TrackedInventory == ESAS_TrackedInventoryCategory::None) return;

	TMap<FPrimaryAssetId, int32> Snapshot;
	Inventory->GetAllItemTotals(Snapshot);

	//Key Value Pair
	for (const TPair<FPrimaryAssetId, int32>& KVP : Snapshot)
	{
		const FPrimaryAssetId& ItemId = KVP.Key;
		const int32 Amount = KVP.Value;

		if (!ItemId.IsValid()) continue;
		if (Amount == 0) continue;

		ApplyDeltaAndBroadcast(TrackedInventory, ItemId, Amount);
	}
}

void USAS_InventoryManagerComponent::RemoveEntireInventoryFromTrackedInventory(ESAS_TrackedInventoryCategory TrackedInventory, USAS_InventoryComponent* Inventory)
{
	if (!Inventory || TrackedInventory == ESAS_TrackedInventoryCategory::None) return;

	TMap<FPrimaryAssetId, int32> Snapshot;
	Inventory->GetAllItemTotals(Snapshot);

	//Key Value Pair
	for (const TPair<FPrimaryAssetId, int32>& KVP : Snapshot)
	{
		const FPrimaryAssetId& ItemId = KVP.Key;
		const int32 Amount = KVP.Value;

		if (!ItemId.IsValid()) continue;
		if (Amount == 0) continue;

		ApplyDeltaAndBroadcast(TrackedInventory, ItemId, -Amount);
	}
}

void USAS_InventoryManagerComponent::ApplyDeltaAndBroadcast(ESAS_TrackedInventoryCategory InventoryToModify, const FPrimaryAssetId& ItemId, int32 Delta)
{
	if (!ItemId.IsValid() || Delta == 0) return;

	TMap<FPrimaryAssetId, int32>* TargetInventoryTotals = nullptr;
	FOnTeamInventoryChanged* DispatcherToUse = nullptr;
	switch (InventoryToModify)
	{
	case ESAS_TrackedInventoryCategory::Stockpile:
		TargetInventoryTotals = &TeamStockpileTotals;
		DispatcherToUse = &OnTeamStockpileInventoryChanged;
		break;
	case ESAS_TrackedInventoryCategory::Transit:
		TargetInventoryTotals = &TeamTransitTotals;
		DispatcherToUse = &OnTeamTransitInventoryChanged;
		break;
	case ESAS_TrackedInventoryCategory::RefinerInput:
		TargetInventoryTotals = &TeamRefinerInputTotals;
		DispatcherToUse = &OnTeamRefinerInputInventoryChanged;
		break;
	default: return;
	}

	if (!ensure(TargetInventoryTotals && DispatcherToUse)) return;

	int32& TotalRef = TargetInventoryTotals->FindOrAdd(ItemId);
	TotalRef += Delta;

	int32 NewTotal = TotalRef;


	if (NewTotal <= 0)
	{
		TargetInventoryTotals->Remove(ItemId);
		NewTotal = 0;
	}

		DispatcherToUse->Broadcast(AssignedTeam, ItemId, Delta, NewTotal);

}

