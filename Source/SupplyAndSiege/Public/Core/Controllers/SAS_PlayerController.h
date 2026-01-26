// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	EMousePosition GetMouseEdgePosition(
		float HorizontalEdgeDistance,
		float VerticalEdgeDistance
	) const;

};
