// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ASAS_PlayerPawn;



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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (Bitmask, BitmaskEnum = "/Script/SUPPLYANDSIEGE.ERotationBlocker"))
	int32 RotationBlockerMask = 0;

	ASAS_PlayerController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	FMouseEdgeResult GetMouseEdgePosition(
		float HorizontalEdgeDistance,
		float VerticalEdgeDistance
	) const;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller State")
	EControllerAction CurrentAction = EControllerAction::None;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_PlayerMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_ToggleRotate;

	UPROPERTY(BlueprintReadOnly)
	FMouseEdgeResult MouseEdgeResult = FMouseEdgeResult();

	UPROPERTY(EditDefaultsOnly,Category = "Movement")
	FVector2D ScreenMovementBounds = FVector2D(50.f, 50.f);

	UPROPERTY(BlueprintReadOnly)
	bool OverrideMoveByInputAction = false;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MoveInputActionAxis = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	ASAS_PlayerPawn* PlayerPawn;

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void MoveUpdated(const FInputActionValue& Value);
	void MoveEnded(const FInputActionValue& Value);

	UFUNCTION()
	void OnRotationToggleStarted(const FInputActionValue& Value);

	UFUNCTION()
	void OnRotationToggleEnded(const FInputActionValue& Value);

	UFUNCTION()
	void Move();


};
