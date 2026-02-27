// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAS_AI_Controller.generated.h"

class UStateTreeAIComponent;

/**
 * AIC for units.
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_AI_Controller : public AAIController
{
	GENERATED_BODY()

public:
	ASAS_AI_Controller();

protected:
	/** Runs the behavior StateTree for this unit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<UStateTreeAIComponent> StateTreeComp;
};
