


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
	if (!Item || Quantity <= 0) return 0;

	//TODO: Still need to set it up so that slots are updated with the max stack for the slot based on inventory type and item in the slot.
	int32 Remaining = Quantity;

	for (FSAS_InventorySlot& Slot : Slots)
	{
		if (Remaining <= 0) break;
		if (Slot.Item != Item) continue;
		if (Slot.RemainingSpaceInSlot <= 0) continue;

		int32 AmountToAddToSlot = FMath::Min(Remaining, Slot.RemainingSpaceInSlot);
		Slot.Quantity += AmountToAddToSlot;
		Remaining += AmountToAddToSlot;
	}

	if (Remaining > 0)
	{
		for (FSAS_InventorySlot& Slot : Slots)
		{
			if (Remaining <= 0) break;
			if (!Slot.IsEmpty()) continue;

			//TODO: Need to update this so that when adding to a new slot it evaluates the inventory type and item to set the new max stack for this slot.
			Slot.Item = Item;
			Slot.Quantity = Remaining;
			Remaining = 0;
		}
	}
	
	const int32 Added = Quantity - Remaining;

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



