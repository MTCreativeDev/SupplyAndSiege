// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SAS_PlayerPawn.generated.h"

class UFloatingPawnMovement;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_PlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASAS_PlayerPawn();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	TObjectPtr<UFloatingPawnMovement> FloatingMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float MovementSpeed = 5000.f;

public:	

	UFUNCTION(BlueprintCallable)
	void Move(FVector2D MovementAxis);

	UFUNCTION(BlueprintCallable)
	void Rotate(float RotationChange);



};
