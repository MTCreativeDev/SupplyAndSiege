// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_PlayerController.generated.h"

/**
 * 
 */

//Struct used to return the MousePosition and the MovementAxis
USTRUCT(BlueprintType)
struct FMouseEdgeResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EMousePosition MousePosition = EMousePosition::None;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MovementAxis = FVector2D::ZeroVector;

};


UCLASS()
class SUPPLYANDSIEGE_API ASAS_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (Bitmask, BitmaskEnum = "/Script/SUPPLYANDSIEGE.EMovementBlocker"))
	int32 MovementBlockerMask = 0;

	UFUNCTION(BlueprintCallable)
	FMouseEdgeResult GetMouseEdgePosition(
		float HorizontalEdgeDistance,
		float VerticalEdgeDistance
	) const;

};
