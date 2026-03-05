
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryManagerComponent.h"
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
	const int32 Added = AddItem_Internal(Item, Quantity);

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

int32 USAS_InventoryComponent::HasInventorySpace(UItemDefinitionPrimaryData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0) return Quantity;
	int32 Remaining = Quantity;

	const int32 MaxStack = GetMaxStack(Item);
	//Existing Slots
	for (FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) return 0;
		if (Slot.IsEmpty()) continue;
		if (Slot.Item != Item) continue;

		const int32 SpaceLeft = FMath::Max(0, MaxStack - Slot.Quantity);
		if (SpaceLeft <= 0) continue;

		Remaining -= FMath::Min(SpaceLeft, Remaining);
	}


	// Empty Slots

	for (FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) return 0;
		if (!Slot.IsEmpty()) continue;

		Remaining -= FMath::Min(MaxStack, Remaining);
	}
	return Remaining;
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
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				TEXT("Inventory Component Could Not Find Unit Information Component")
			);
		}
		return;
	}

	UnitInfoComponent = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!UnitInfoComponent)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				TEXT("Inventory Component Could Not Find Unit Information Component")
			);
		}
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



