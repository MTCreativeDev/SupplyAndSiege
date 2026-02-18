// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SAS_GameState.h"
#include "Core/Components/SAS_InventoryManagerComponent.h"

ASAS_GameState::ASAS_GameState()
{
	//WARNING: This only works for the single player game.
	
	InventoryManagerComponentTeam1 = CreateDefaultSubobject<USAS_InventoryManagerComponent>(TEXT("InventoryManagerTeam1"));
	InventoryManagerComponentTeam2 = CreateDefaultSubobject<USAS_InventoryManagerComponent>(TEXT("InventoryManagerTeam2"));
	
	InventoryManagerComponentTeam1->AssignedTeam = ESAS_Team::Team1;
	InventoryManagerComponentTeam2->AssignedTeam = ESAS_Team::Team2;
}

USAS_InventoryManagerComponent* ASAS_GameState::GetInventoryManagerForTeam(ESAS_Team Team) const
{
	switch (Team)
	{
	case ESAS_Team::Team1: return InventoryManagerComponentTeam1;
	case ESAS_Team::Team2: return InventoryManagerComponentTeam2;
	default: return nullptr;
	}
}
