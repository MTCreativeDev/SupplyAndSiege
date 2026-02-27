// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SAS_GameMode.h"

#include "Core/AI/SAS_OpponentPawn.h"
#include "Kismet/GameplayStatics.h"

ASAS_GameMode::ASAS_GameMode() 
{
	AIOpponentClass = ASAS_OpponentPawn::StaticClass();
}

void ASAS_GameMode::StartPlay()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.bDeferConstruction = true;
	ASAS_OpponentPawn* AIOpponent = GetWorld()->SpawnActor<ASAS_OpponentPawn>(AIOpponentClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnInfo);

	AActor* PlayerStart = FindPlayerStart(AIOpponent->GetController());

	UGameplayStatics::FinishSpawningActor(AIOpponent, PlayerStart->GetTransform());
	
	Super::StartPlay();
}
