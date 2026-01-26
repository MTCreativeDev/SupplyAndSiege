// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Pawns/SAS_PlayerPawn.h"

// Sets default values
ASAS_PlayerPawn::ASAS_PlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASAS_PlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASAS_PlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASAS_PlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

