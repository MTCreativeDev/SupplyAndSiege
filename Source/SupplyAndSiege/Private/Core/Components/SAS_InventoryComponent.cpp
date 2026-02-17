


#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"
#include "Core/Components/SAS_UnitInformationComponent.h"


USAS_InventoryComponent::USAS_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	AssignedTeam = NewTeam;

	//TODO: Need to set it up so that this is broadcast to the new team so they see their new resource count etc. Also remove any assigned resources from the old team, but likely irrelevant in this situation.
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



