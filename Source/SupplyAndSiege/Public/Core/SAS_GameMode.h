// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SAS_GameMode.generated.h"

class ASAS_OpponentPawn;
/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_GameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASAS_GameMode();
	virtual void StartPlay() override;

	/** The class of AI Opponent to spawn */
	UPROPERTY(EditAnywhere, NoClear, BlueprintReadOnly, Category=Classes)
	TSubclassOf<ASAS_OpponentPawn> AIOpponentClass;
};
