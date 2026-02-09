// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_SelectablePawn.generated.h"

class USAS_UnitInformationComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_SelectablePawn : public APawn
{
	GENERATED_BODY()

public:
	ASAS_SelectablePawn();

	virtual UPawnMovementComponent* GetMovementComponent() const override;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void DestroySelf();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAS_Unit", meta = (ExposeOnSpawn = true))
	ESAS_Team AssignTeamOnSpawn = ESAS_Team::None;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USAS_UnitInformationComponent* UnitInformationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UFloatingPawnMovement* MovementComponent;

};
