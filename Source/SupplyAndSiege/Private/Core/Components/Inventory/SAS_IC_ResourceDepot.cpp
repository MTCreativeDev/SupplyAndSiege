


#include "Core/Components/Inventory/SAS_IC_ResourceDepot.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "GameFramework/GameStateBase.h"

void USAS_IC_ResourceDepot::GetOfferedItems(TMap<UItemDefinitionPrimaryData*, int32>& OutOffers) const
{
	OutOffers.Reset();
	if (!bBroadcastOffers) return;

	TSet<TObjectPtr<UItemDefinitionPrimaryData>> UniqueItems;

	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty() || !IsValid(Slot.Item)) continue;
		UniqueItems.Add(Slot.Item);
	}

	for (const TObjectPtr<UItemDefinitionPrimaryData>& ItemPtr : UniqueItems)
	{
		UItemDefinitionPrimaryData* Item = ItemPtr.Get();
		if (!IsValid(Item)) continue;

		const int32 AvailableAmount = GetUnreservedItemAmount(Item);
		if (AvailableAmount > 0)
		{
			OutOffers.Add(Item, AvailableAmount);
		}
	}
}

void USAS_IC_ResourceDepot::GetRequestedRemovals(TMap<UItemDefinitionPrimaryData*, int32>& OutRequests) const
{
	OutRequests.Reset();

	if (!bRequestRemovalWhenNotEmpty) return;

	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty()) continue;
		if (!IsValid(Slot.Item)) continue;

		const int32 AvailableAmount = GetUnreservedItemAmount(Slot.Item);
		if (AvailableAmount <= 0) continue;

		const int32 Priority = GetRemovalPriorityForQuantity(AvailableAmount);
		if (Priority <= 0) continue;

		OutRequests.FindOrAdd(Slot.Item) = AvailableAmount;
	}
}

int32 USAS_IC_ResourceDepot::GetRemovalPriorityForQuantity(int32 Quantity) const
{
	int32 BestPriority = 0;

	for (const FSAS_InventoryPriorityThreshold& Threshold : RemovalPriorityThresholds)
	{
		if (Quantity >= Threshold.MinimumQuantity)
		{
			BestPriority = Threshold.Priority;
		}
		else
		{
			break;
		}
	}

	return BestPriority;
}

int32 USAS_IC_ResourceDepot::AddItem(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	const int32 Added = Super::AddItem(Item, Quantity);

	if (Added > 0)
	{
		RefreshLogisticsState();
	}
	return Added;
}

int32 USAS_IC_ResourceDepot::RemoveItem(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	const int32 Removed = Super::RemoveItem(Item, Quantity);

	if (Removed > 0)
	{
		RefreshLogisticsState();
	}

	return Removed;
}

void USAS_IC_ResourceDepot::BeginPlay()
{
	Super::BeginPlay();

	SortPriorityThresholds();

	UWorld* World = GetWorld();
	if (!World) return;
	AGameStateBase* GS = World->GetGameState();
	if (!GS) return;

	LogisticsManager = GS->FindComponentByClass<USAS_LogisticsManagerComponent>();

	RefreshLogisticsState();
}

void USAS_IC_ResourceDepot::SortPriorityThresholds()
{
	RemovalPriorityThresholds.Sort([](const FSAS_InventoryPriorityThreshold& A, const FSAS_InventoryPriorityThreshold& B)
		{
			return A.MinimumQuantity < B.MinimumQuantity;
		});
}

void USAS_IC_ResourceDepot::RefreshLogisticsState()
{
	if (!LogisticsManager) return;

	TArray<FSAS_LogisticsOffering> Offerings;
	GetCurrentOfferings(Offerings);

	LogisticsManager->UpdateInventoryOfferings(this, Offerings);
	
}

void USAS_IC_ResourceDepot::GetCurrentOfferings(TArray<FSAS_LogisticsOffering>& OutOfferings) const
{
	OutOfferings.Reset();

	if (!bBroadcastOffers) return;

	TSet<TObjectPtr<UItemDefinitionPrimaryData>> UniqueItems;

	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty()) continue;
		if (!IsValid(Slot.Item)) continue;

		UniqueItems.Add(Slot.Item);
	}

	for (UItemDefinitionPrimaryData* Item : UniqueItems)
	{
		if (!IsValid(Item)) continue;

		const int32 AvailableAmount = GetUnreservedItemAmount(Item);
		if (AvailableAmount <= 0) continue;

		FSAS_LogisticsOffering NewOffering;
		NewOffering.SourceInventory = const_cast<USAS_IC_ResourceDepot*>(this);
		NewOffering.Item = Item;
		NewOffering.Quantity = AvailableAmount;

		OutOfferings.Add(NewOffering);
	}
}

