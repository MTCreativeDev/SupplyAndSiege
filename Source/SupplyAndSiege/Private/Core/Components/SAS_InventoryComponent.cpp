


#include "Core/Components/SAS_InventoryComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"


USAS_InventoryComponent::USAS_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}





void USAS_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

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
	AssignedTeam = UnitInfoComponent->AssignedTeam;

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
}



