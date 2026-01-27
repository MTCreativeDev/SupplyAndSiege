// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Pawns/SAS_PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ASAS_PlayerPawn::ASAS_PlayerPawn()
{
	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

UFUNCTION(BlueprintCallable)
void ASAS_PlayerPawn::Move(FVector2D MovementAxis)
{
	if (!FloatingMovement)
	{
		return;

	}

	const FVector Forward = GetActorForwardVector();
	const FVector Right = GetActorRightVector();

	FVector MovementVector = (Forward * MovementAxis.Y) + (Right * MovementAxis.X);

	MovementVector = MovementVector.GetClampedToMaxSize(1.f);

	MovementVector *= MovementSpeed;

	FloatingMovement->AddInputVector(MovementVector, false);
}

UFUNCTION(BlueprintCallable)
void ASAS_PlayerPawn::Rotate(float RotationChange)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += RotationChange;
	SetActorRotation(NewRotation);
}


