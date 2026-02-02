// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Pawns/SAS_SelectablePawn.h"

// Sets default values
ASAS_SelectablePawn::ASAS_SelectablePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASAS_SelectablePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASAS_SelectablePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASAS_SelectablePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

