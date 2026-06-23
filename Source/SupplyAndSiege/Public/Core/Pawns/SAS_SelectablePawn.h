// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Core/SAS_Enumerators.h"
#include "Core/Interfaces/SAS_PlayerVisibilityInterface.h"
#include "Core/Interfaces/SAS_ClickTarget.h"
#include "SAS_SelectablePawn.generated.h"

class USAS_UnitInformationComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class USAS_UnitSightComponent;
class USAS_HungerComponent;


UCLASS()
class SUPPLYANDSIEGE_API ASAS_SelectablePawn : public APawn, public ISAS_PlayerVisibilityInterface, public ISAS_ClickTarget
{
	GENERATED_BODY()

public:
	ASAS_SelectablePawn();

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	virtual void SetVisibleToPlayer_Implementation(bool bVisible) override;
	virtual bool IsVisibleToPlayer_Implementation() const override;

	virtual ESAS_ClickTargetType GetClickTargetType_Implementation() const override;


protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit_Pawn")
	USAS_HungerComponent* HungerComponent;


private:
	UPROPERTY()
	bool bVisibleToPlayer = true;

};
