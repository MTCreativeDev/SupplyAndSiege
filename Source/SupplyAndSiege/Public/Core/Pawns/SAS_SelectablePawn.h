// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Core/SAS_Enumerators.h"
#include "Core/Interfaces/SAS_PlayerVisibilityInterface.h"
#include "SAS_SelectablePawn.generated.h"

class USAS_UnitInformationComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class USAS_UnitSightComponent;


UCLASS()
class SUPPLYANDSIEGE_API ASAS_SelectablePawn : public APawn, public ISAS_PlayerVisibilityInterface
{
	GENERATED_BODY()

public:
	ASAS_SelectablePawn();

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	virtual void SetVisibleToPlayer_Implementation(bool bVisible) override;
	virtual bool IsVisibleToPlayer_Implementation() const override;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "SAS_Unit")
	void DestroySelf();

	void ApplyInitialTeamVisibility();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USAS_UnitSightComponent* UnitSightComponent;


private:
	UPROPERTY()
	bool bVisibleToPlayer = true;

};
