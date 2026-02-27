// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AI/SAS_OpponentPawn.h"

#include "Core/AI/SAS_OpponentAIC.h"


ASAS_OpponentPawn::ASAS_OpponentPawn()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = ASAS_OpponentAIC::StaticClass();
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}
