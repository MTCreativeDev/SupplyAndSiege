// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAS_OpponentAIC.generated.h"

class USAS_UnitManagerComponent;
class UStateTreeAIComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_OpponentAIC : public AAIController
{
	GENERATED_BODY()

public:
	ASAS_OpponentAIC();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USAS_UnitManagerComponent> UnitManagerComp;

	/** Runs the behavior StateTree for this unit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStateTreeAIComponent> StateTreeComp;
};
