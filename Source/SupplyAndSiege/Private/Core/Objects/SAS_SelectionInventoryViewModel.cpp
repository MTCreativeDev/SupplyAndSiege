

#include "Core/Objects/SAS_SelectionInventoryViewModel.h"
#include "Core/Components/SAS_UnitManagerComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"



void USAS_SelectionInventoryViewModel::Initialize(USAS_UnitManagerComponent* InUnitManager)
{
	if (!InUnitManager) return;

	Shutdown();
	UnitManager = InUnitManager;
	UnitManager->OnUnitSelectionChange.AddDynamic(this, &USAS_SelectionInventoryViewModel::HandleSelectedUnitsChanged);

	RebuildModel();
}

void USAS_SelectionInventoryViewModel::Shutdown()
{
	if (UnitManager)
	{
		UnitManager->OnUnitSelectionChange.RemoveDynamic(this, &USAS_SelectionInventoryViewModel::HandleSelectedUnitsChanged);
	}
	UnbindAllObservedInventories();

	UnitManager = nullptr;
	SelectedUnits.Empty();
	GroupTotals.Empty();
	Mode = ESAS_SelectionInventoryMode::None;
}

void USAS_SelectionInventoryViewModel::HandleSelectedUnitsChanged(const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>& NewSelection)
{
	SelectedUnits.Empty();
	SelectedUnits.Reserve(NewSelection.Num());

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfo : NewSelection)
	{
		if (!UnitInfo.IsValid()) continue;
		SelectedUnits.Add(UnitInfo);

	}

	RebuildModel();
	OnSelectionInventoryModelChanged.Broadcast();

}

void USAS_SelectionInventoryViewModel::RebuildObservedInventories()
{
	ObservedInventories.Empty();

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfoPtr : SelectedUnits)
	{
		if (!UnitInfoPtr.IsValid()) continue;

		AActor* Owner = UnitInfoPtr->GetOwner();
		if (!Owner) continue;

		TArray<USAS_InventoryComponent*> Inventories;
		Owner->GetComponents<USAS_InventoryComponent>(Inventories);

		for (USAS_InventoryComponent* Inv : Inventories)
		{
			if (!Inv) continue;
			ObservedInventories.Add(Inv);
		}
	}
}

void USAS_SelectionInventoryViewModel::BindObservedInventories()
{
	for (const TWeakObjectPtr<USAS_InventoryComponent>& InvPtr : ObservedInventories)
	{
		if (!InvPtr.IsValid()) continue;

		InvPtr->OnInventoryChanged.AddDynamic(this, &USAS_SelectionInventoryViewModel::HandleObservedInventoryChanged);
	}
}

void USAS_SelectionInventoryViewModel::UnbindAllObservedInventories()
{
	for (const TWeakObjectPtr<USAS_InventoryComponent>& InvPtr : ObservedInventories)
	{
		if (!InvPtr.IsValid()) continue;
		InvPtr->OnInventoryChanged.RemoveDynamic(this, &USAS_SelectionInventoryViewModel::HandleObservedInventoryChanged);
	}
	ObservedInventories.Empty();
}

void USAS_SelectionInventoryViewModel::HandleObservedInventoryChanged(USAS_InventoryComponent* Sender, FPrimaryAssetId ItemID, int32 Delta)
{
	if (!Sender) return;
	if (!ObservedInventories.Contains(Sender)) return;
	if (Mode == ESAS_SelectionInventoryMode::None) return;

	//Doesnt rebind everything, just update totals
	RebuildGroupTotals_AllInventories();

	OnSelectionInventoryModelChanged.Broadcast();
}

void USAS_SelectionInventoryViewModel::RebuildModel()
{
	UnbindAllObservedInventories();

	const int32 NumSelected = SelectedUnits.Num();

	if (NumSelected == 0)
	{
		Mode = ESAS_SelectionInventoryMode::None;
		GroupTotals.Empty();
		return;
	}

	Mode = (NumSelected == 1) ? ESAS_SelectionInventoryMode::Single : ESAS_SelectionInventoryMode::Group;

	RebuildObservedInventories();
	BindObservedInventories();

	RebuildGroupTotals_AllInventories();
}

void USAS_SelectionInventoryViewModel::RebuildGroupTotals_AllInventories()
{
	GroupTotals.Empty();

	TMap<FPrimaryAssetId, int32> TempTotals;

	for (const TWeakObjectPtr<USAS_InventoryComponent>& InvPtr : ObservedInventories)
	{
		if (!InvPtr.IsValid()) continue;

		const USAS_InventoryComponent* Inv = InvPtr.Get();
		
		TempTotals.Reset();
		Inv->GetAllItemTotals(TempTotals);

		for (const TPair<FPrimaryAssetId, int32>& Pair : TempTotals)
		{
			if (!Pair.Key.IsValid()) continue;
			if (Pair.Value <= 0) continue;

			GroupTotals.FindOrAdd(Pair.Key) += Pair.Value;
		}
	}
}
