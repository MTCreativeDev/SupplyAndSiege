// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AI/SAS_OpponentAIC.h"

#include "Components/StateTreeAIComponent.h"
#include "Core/Components/SAS_UnitManagerComponent.h"

ASAS_OpponentAIC::ASAS_OpponentAIC()
{
	UnitManagerComp = CreateDefaultSubobject<USAS_UnitManagerComponent>(TEXT("UnitManagerComp"));
	UnitManagerComp->SetTeam(ESAS_Team::Team2);
	
	StateTreeComp = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeComp"));
}
