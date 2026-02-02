// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Pawns/SAS_SelectablePawn.h"
#include "Core/Components/SAS_UnitInformationComponent.h"

ASAS_SelectablePawn::ASAS_SelectablePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	UnitInformationComponent = CreateDefaultSubobject<USAS_UnitInformationComponent>(TEXT("UnitInformationComponent"));

}

void ASAS_SelectablePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

