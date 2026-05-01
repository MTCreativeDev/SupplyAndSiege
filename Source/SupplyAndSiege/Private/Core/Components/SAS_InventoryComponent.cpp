
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryManagerComponent.h"
#include "Misc/Structs/SAS_InventoryReservationHandle.h"
#include "Core/SAS_GameState.h"


USAS_InventoryComponent::USAS_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ESAS_Team USAS_InventoryComponent::GetAssignedTeam()
{
	return AssignedTeam;
}

USAS_InventoryProfileData* USAS_InventoryComponent::GetInventoryProfile()
{
	return InventoryProfile;
}

void USAS_InventoryComponent::GetAllItemTotals(TMap<FPrimaryAssetId, int32>& OutTotals) const
{
	OutTotals.Reset();

	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty()) continue;

		const FPrimaryAssetId ItemId = Slot.Item->GetPrimaryAssetId();
		if (!ItemId.IsValid()) continue;

		int32& Total = OutTotals.FindOrAdd(ItemId);
		Total += Slot.Quantity;
	}
}

int32 USAS_InventoryComponent::AddItem(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0) return 0;

	const int32 Overflow = GetReservationAwareOverflow(Item, Quantity);
	const int32 SafeQuantityToAdd = Quantity - Overflow;

	if (SafeQuantityToAdd <= 0) return 0;

	const int32 Added = AddItem_Internal(Item, SafeQuantityToAdd);

	if (Added > 0)
	{
		const FPrimaryAssetId ItemId = Item->GetPrimaryAssetId();
		if (ItemId.IsValid())
		{
			OnInventoryChanged.Broadcast(this, ItemId, Added);
		}
	}
	return Added;
	
}

int32 USAS_InventoryComponent::GetMaxStack(const UItemDefinitionPrimaryData* Item) const
{
	if (!Item) return 0;
	if (!InventoryProfile) return 0;

	if (InventoryProfile->bCanOnlyStoreOneResource)
	{
		if (!InventoryProfile->OnlyStorableResource) return 0;
		if (InventoryProfile->OnlyStorableResource != Item) return 0;
	}
	if (const int32* Override = InventoryProfile->MaxPerSlotOverrides.Find(const_cast<UItemDefinitionPrimaryData*>(Item)))
	{
		return FMath::Max(0, *Override);
	}
	return FMath::Max(0, InventoryProfile->DefaultMaxPerSlot);
}

bool USAS_InventoryComponent::CreateReservationRecord(ESAS_InventoryReservationType ReservationType, UItemDefinitionPrimaryData* Item, int32 Quantity, UObject* Claimer, FSAS_InventoryReservationHandle& OutHandle)
{
	OutHandle.Reset();

	if (!Item || Quantity <= 0 || ReservationType == ESAS_InventoryReservationType::None)
	{
		return false;
	}

	FSAS_InventoryReservationRecord NewRecord;
	NewRecord.ReservationID = FGuid::NewGuid();
	NewRecord.ReservationType = ReservationType;
	NewRecord.Item = Item;
	NewRecord.Quantity = Quantity;
	NewRecord.Claimer = Claimer;

	if (!NewRecord.IsValid())
	{
		return false;
	}

	ActiveReservations.Add(NewRecord.ReservationID, NewRecord);
	OutHandle.ReservationID = NewRecord.ReservationID;

	return true;
}

bool USAS_InventoryComponent::GetReservationRecord(FSAS_InventoryReservationHandle Handle, FSAS_InventoryReservationRecord*& OutRecord)
{
	OutRecord = nullptr;

	if (!Handle.IsValid())
	{
		return false;
	}

	FSAS_InventoryReservationRecord* FoundRecord = ActiveReservations.Find(Handle.ReservationID);
	if (!FoundRecord || !FoundRecord->IsValid())
	{
		return false;
	}

	OutRecord = FoundRecord;
	return true;
}

int32 USAS_InventoryComponent::GetReservationAwareOverflow(UItemDefinitionPrimaryData* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0) return Quantity;

	const int32 AvailableCapacity = GetAvailableInboundCapacity(Item, Quantity);
	return FMath::Max(0, Quantity - AvailableCapacity);
}

int32 USAS_InventoryComponent::RemoveItem(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	const int32 Removed = RemoveItem_Internal(Item,Quantity);

	if (Removed > 0)
	{
		const FPrimaryAssetId ItemId = Item->GetPrimaryAssetId();
		if (ItemId.IsValid())
		{
			OnInventoryChanged.Broadcast(this, ItemId, -Removed);
		}
	}
	return Removed;
}

int32 USAS_InventoryComponent::GetPhysicalOverflow(UItemDefinitionPrimaryData* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0) return Quantity;
	int32 Remaining = Quantity;

	const int32 MaxStack = GetMaxStack(Item);
	//Existing Slots
	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) return 0;
		if (Slot.IsEmpty()) continue;
		if (Slot.Item != Item) continue;

		const int32 SpaceLeft = FMath::Max(0, MaxStack - Slot.Quantity);
		if (SpaceLeft <= 0) continue;

		Remaining -= FMath::Min(SpaceLeft, Remaining);
	}


	// Empty Slots

	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) return 0;
		if (!Slot.IsEmpty()) continue;

		Remaining -= FMath::Min(MaxStack, Remaining);
	}
	return Remaining;
}

void USAS_InventoryComponent::SetInventoryProfile(USAS_InventoryProfileData* NewProfile)
{
	if (!NewProfile) return;
	InventoryProfile = NewProfile;
}

int32 USAS_InventoryComponent::GetTotalItemAmount(UItemDefinitionPrimaryData* Item) const
{
	if (!Item) return 0;

	int32 Total = 0;
	for (const FSAS_InventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty()) continue;
		if (Slot.Item != Item) continue;

		Total += Slot.Quantity;
	}
	return Total;
}

int32 USAS_InventoryComponent::GetReservedOutboundAmount(UItemDefinitionPrimaryData* Item) const
{
	if (!Item) return 0;

	int32 Total = 0;

	for (const TPair<FGuid, FSAS_InventoryReservationRecord>& Pair : ActiveReservations)
	{
		const FSAS_InventoryReservationRecord& Record = Pair.Value;
		if (!Record.IsValid()) continue;
		if (Record.ReservationType != ESAS_InventoryReservationType::Outbound) continue;
		if (Record.Item != Item) continue;

		Total += Record.Quantity;
	}
	return Total;
}

int32 USAS_InventoryComponent::GetReservedInboundAmount(UItemDefinitionPrimaryData* Item) const
{
	if (!Item) return 0;

	int32 Total = 0;

	for (const TPair<FGuid, FSAS_InventoryReservationRecord>& Pair : ActiveReservations)
	{
		const FSAS_InventoryReservationRecord& Record = Pair.Value;
		if (!Record.IsValid()) continue;
		if (Record.ReservationType != ESAS_InventoryReservationType::Inbound) continue;
		if (Record.Item != Item) continue;

		Total += Record.Quantity;
	}

	return Total;
}

int32 USAS_InventoryComponent::GetUnreservedItemAmount(UItemDefinitionPrimaryData* Item) const
{
	if (!Item) return 0;

	const int32 PhysicalAmount = GetTotalItemAmount(Item);
	const int32 ReservedOutbound = GetReservedOutboundAmount(Item);

	return FMath::Max(0, PhysicalAmount - ReservedOutbound);
}

int32 USAS_InventoryComponent::GetAvailableInboundCapacity(UItemDefinitionPrimaryData* Item, int32 RequestedQuantity) const
{
	if (!Item || RequestedQuantity <= 0) return 0;

	const int32 TestQuantity = RequestedQuantity + GetReservedInboundAmount(Item);
	const int32 Overflow = GetPhysicalOverflow(Item, TestQuantity);
	const int32 PhysicalCapacity = TestQuantity - Overflow;
	const int32 ReservedInbound = GetReservedInboundAmount(Item);

	return FMath::Max(0, PhysicalCapacity - ReservedInbound);
}

bool USAS_InventoryComponent::ReserveOutbound(UItemDefinitionPrimaryData* Item, int32 RequestedQuantity, UObject* Claimer, FSAS_InventoryReservationHandle& OutHandle, int32& OutReservedQuantity)
{
	OutHandle.Reset();
	OutReservedQuantity = 0;

	if (!Item || RequestedQuantity <= 0)
	{
		return false;
	}

	const int32 AvailableAmount = GetUnreservedItemAmount(Item);
	const int32 QuantityToReserve = FMath::Min(RequestedQuantity, AvailableAmount);

	if (QuantityToReserve <= 0)
	{
		return false;
	}

	if (!CreateReservationRecord(ESAS_InventoryReservationType::Outbound, Item, QuantityToReserve, Claimer, OutHandle))
	{
		return false;
	}

	OutReservedQuantity = QuantityToReserve;
	return true;
}

bool USAS_InventoryComponent::ReserveInbound(UItemDefinitionPrimaryData* Item, int32 RequestedQuantity, UObject* Claimer, FSAS_InventoryReservationHandle& OutHandle, int32& OutReservedQuantity)
{
	OutHandle.Reset();
	OutReservedQuantity = 0;

	if (!Item || RequestedQuantity <= 0)
	{
		return false;
	}

	const int32 AvailableCapacity = GetAvailableInboundCapacity(Item, RequestedQuantity);
	const int32 QuantityToReserve = FMath::Min(RequestedQuantity, AvailableCapacity);

	if (QuantityToReserve <= 0)
	{
		return false;
	}

	if (!CreateReservationRecord(ESAS_InventoryReservationType::Inbound, Item, QuantityToReserve, Claimer, OutHandle))
	{
		return false;
	}

	OutReservedQuantity = QuantityToReserve;
	return true;
}

bool USAS_InventoryComponent::ReleaseReservation(FSAS_InventoryReservationHandle Handle)
{
	if (!Handle.IsValid())
	{
		return false;
	}

	return ActiveReservations.Remove(Handle.ReservationID) > 0;
}

bool USAS_InventoryComponent::FinalizeOutboundPickup(FSAS_InventoryReservationHandle Handle, int32& OutRemovedQuantity)
{
	OutRemovedQuantity = 0;

	FSAS_InventoryReservationRecord* Record = nullptr;
	if (!GetReservationRecord(Handle, Record) || !Record)
	{
		return false;
	}

	if (Record->ReservationType != ESAS_InventoryReservationType::Outbound)
	{
		return false;
	}

	const int32 Removed = RemoveItem(Record->Item, Record->Quantity);
	if (Removed <= 0)
	{
		return false;
	}

	OutRemovedQuantity = Removed;
	ActiveReservations.Remove(Handle.ReservationID);
	return true;
}

bool USAS_InventoryComponent::FinalizeInboundDropoff(FSAS_InventoryReservationHandle Handle, int32 QuantityToDeliver, int32& OutAddedQuantity)
{
	OutAddedQuantity = 0;

	FSAS_InventoryReservationRecord* Record = nullptr;
	if (!GetReservationRecord(Handle, Record) || !Record)
	{
		return false;
	}

	if (Record->ReservationType != ESAS_InventoryReservationType::Inbound)
	{
		return false;
	}

	if (QuantityToDeliver <= 0)
	{
		return false;
	}

	const int32 QuantityToAdd = FMath::Min(QuantityToDeliver, Record->Quantity);
	const int32 Added = AddItem_Internal(Record->Item, QuantityToAdd);

	if (Added <= 0)
	{
		return false;
	}

	const FPrimaryAssetId ItemId = Record->Item->GetPrimaryAssetId();
	if (ItemId.IsValid())
	{
		OnInventoryChanged.Broadcast(this, ItemId, Added);
	}

	OutAddedQuantity = Added;
	ActiveReservations.Remove(Handle.ReservationID);
	return true;
}

void USAS_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	GetUnitInformationAndBindToTeamChange();

	CreateSlotsFromProfile();

}

void USAS_InventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UnitInfoComponent)
	{
		UnitInfoComponent->NotifyTeamChange.RemoveDynamic(this, &USAS_InventoryComponent::HandleTeamChanged);
	}

	ActiveReservations.Reset();

	Super::EndPlay(EndPlayReason);
}

void USAS_InventoryComponent::HandleTeamChanged(ESAS_Team NewTeam)
{
	if (AssignedTeam == NewTeam) return;

	ASAS_GameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASAS_GameState>() : nullptr;

	if (!GameState)
	{
		AssignedTeam = NewTeam;
		return;
	}
	if (AssignedTeam == ESAS_Team::Team1 || AssignedTeam == ESAS_Team::Team2)
	{
		USAS_InventoryManagerComponent* OldInventoryManagerComponent = GameState->GetInventoryManagerForTeam(AssignedTeam);
		check(OldInventoryManagerComponent);
		OldInventoryManagerComponent->UnregisterTeamInventory(this);
	}

	AssignedTeam = NewTeam;

	if (NewTeam == ESAS_Team::Team1 || NewTeam == ESAS_Team::Team2)
	{
		USAS_InventoryManagerComponent* NewInventoryManagerComponent = GameState->GetInventoryManagerForTeam(NewTeam);
		check(NewInventoryManagerComponent);
		NewInventoryManagerComponent->RegisterTeamInventory(this);
	}

}

void USAS_InventoryComponent::GetUnitInformationAndBindToTeamChange()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UnitInfoComponent = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!UnitInfoComponent)
	{
		return;
	}

	UnitInfoComponent->NotifyTeamChange.AddDynamic(this, &USAS_InventoryComponent::HandleTeamChanged);
	HandleTeamChanged(UnitInfoComponent->AssignedTeam);
		
}

void USAS_InventoryComponent::CreateSlotsFromProfile()
{
	if (!InventoryProfile)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 5.f, FColor::Red,
				TEXT("InventoryComponent: InventoryProfile is null (cannot create slots).")
			);
		}
		return;
	}

	//In case someone does a negative
	const int32 NumSlots = FMath::Max(0, InventoryProfile->NumSlots);

	Slots.SetNum(NumSlots);

	for (FSAS_InventorySlot& Slot : Slots)
	{
		Slot.ClearSlot();
	}

}

int32 USAS_InventoryComponent::AddItem_Internal(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0) return 0;
	int32 Remaining = Quantity;

	//Existing Slots
	for (FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) break;
		if (Slot.IsEmpty()) continue;
		if (Slot.Item != Item) continue;

		const int32 MaxStack = GetMaxStack(Item);
		const int32 SpaceLeft = FMath::Max(0, MaxStack - Slot.Quantity);
		if (SpaceLeft <= 0) continue;

		const int32 AmountToAddToSlot = FMath::Min(Remaining, SpaceLeft);
		Slot.Quantity += AmountToAddToSlot;
		Remaining -= AmountToAddToSlot;
	}


	// Empty Slots

		for (FSAS_InventorySlot& Slot : Slots)
		{
			if (Remaining <= 0) break;
			if (!Slot.IsEmpty()) continue;

			const int32 MaxStack = GetMaxStack(Item);
			const int32 AmountToAddToSlot = FMath::Min(Remaining, MaxStack);

			Slot.Item = Item;
			Slot.Quantity = AmountToAddToSlot;
			
			Remaining -= AmountToAddToSlot;
		}
	return Quantity - Remaining;
}

int32 USAS_InventoryComponent::RemoveItem_Internal(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0) return 0;

	int32 Remaining = Quantity;

	for (FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) break;
		if (Slot.IsEmpty()) continue;
		if (Slot.Item != Item) continue;
		if (Slot.Quantity <= 0) { Slot.ClearSlot(); continue;}

		const int32 AmountToRemove = FMath::Min(Remaining, Slot.Quantity);
		Slot.Quantity -= AmountToRemove;
		Remaining -= AmountToRemove;

		if (Slot.Quantity <= 0)
		{
			Slot.ClearSlot();
		}
	}

	return Quantity - Remaining;
}



