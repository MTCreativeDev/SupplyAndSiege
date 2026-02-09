// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_UnitManagerComponent.h"
#include "AIController.h"


USAS_UnitInformationComponent::USAS_UnitInformationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USAS_UnitInformationComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USAS_UnitInformationComponent::SetTeam(ESAS_Team NewTeam)
{
	if (AssignedTeam == NewTeam)
	{
		return;
	}
	AActor* Owner = GetOwner();
	if (!Owner) return;
	
	if (AssignedUnitManager)
	{
		AssignedUnitManager->RemoveSelectableUnit(Owner);
		AssignedUnitManager = nullptr;
	}


	switch (NewTeam)
	{
	case ESAS_Team::None:
		break;
	case ESAS_Team::EnvironmentTeam:
	{
		//TODO Figure out what to do with environment units
		break;
	}
	case ESAS_Team::Team1:
	{	//WARNING: This will only work for a single player game. 
		APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
		if (!PC) break;
		USAS_UnitManagerComponent* UnitManagerComponent = PC->FindComponentByClass<USAS_UnitManagerComponent>();
		if (!UnitManagerComponent) break;
		AssignedUnitManager = UnitManagerComponent;
		AssignedUnitManager->AssignSelectableUnit(Owner, true);
		break;
	}
	case ESAS_Team::Team2:
	{
		//TODO
		break;
	}

	
	default:
	{
		break;
	}

	}

	AssignedTeam = NewTeam;
}

void USAS_UnitInformationComponent::RemoveUnitFromGame()
{
	if (!AssignedUnitManager) return;
	AActor* Owner = GetOwner();
	if (!Owner) return;

	AssignedUnitManager->RemoveSelectableUnit(Owner);
	//TODO: Make sure to set it up so this unit gets removed from any active selections
}

void USAS_UnitInformationComponent::NotifySelected(ESAS_Team SelectedByTeam)
{
	//Warning: This only works for a single player game. Essentially only the player will be team 1 so no other selections needs to show the ring.
	if (SelectedByTeam == ESAS_Team::Team1) ToggleSelectionRing.Broadcast(true);

}

void USAS_UnitInformationComponent::NotifyDeselected(ESAS_Team DeselectedByTeam)
{
	//Warning: This only works for a single player game. Essentially only the player will be team 1 so no other de-selections needs to show the ring.
	if (DeselectedByTeam == ESAS_Team::Team1) ToggleSelectionRing.Broadcast(false);

}

void USAS_UnitInformationComponent::IssueMoveOrder(FVector WorldLocation)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	AAIController* AI = Pawn ? Cast<AAIController>(Pawn->GetController()) : nullptr;
	if (!AI) return;

	AI->MoveToLocation(WorldLocation, 75.f, true);
}


