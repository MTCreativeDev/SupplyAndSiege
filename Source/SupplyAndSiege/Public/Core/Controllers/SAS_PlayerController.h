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
class USAS_UnitManagerComponent;


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

	//functions

	ASAS_PlayerController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USAS_UnitManagerComponent* UnitManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller State")
	EControllerAction CurrentAction = EControllerAction::None;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_PlayerMovement;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_ToggleRotate;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Select;


	FMouseEdgeResult MouseEdgeResult = FMouseEdgeResult();

	UPROPERTY(EditDefaultsOnly,Category = "Input")
	FVector2D ScreenMovementBounds = FVector2D(50.f, 50.f);

	bool OverrideMoveByInputAction = false;

	FVector2D MoveInputActionAxis = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	ASAS_PlayerPawn* PlayerPawn;

	FVector2D CachedMouseLocation = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float MaxEvaluatedRotationDistance = 400.f;

	// Selection States - I'm using two bools instead of an Enum since I don't expect there to be more than the two states.
	bool bSelecting = false;
	bool bDragging = false;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float SelectingTimeTillDrag = 9.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float SelectionDistanceTillDrag = 8.f;

	float SelectionStartedTime = 0.f;
	FVector2D SelectionStartMousePos = FVector2D::ZeroVector;
	FVector2D CurrentSelectionMousePos = FVector2D::ZeroVector;

	
	//functions

	virtual void SetupInputComponent() override;

	FMouseEdgeResult GetMouseEdgePosition(float HorizontalEdgeDistance,float VerticalEdgeDistance) const;

	void MoveUpdated(const FInputActionValue& Value);
	void MoveEnded(const FInputActionValue& Value);

	void Move();

	void OnRotationToggleStarted(const FInputActionValue& Value);
	void OnRotationToggleEnded(const FInputActionValue& Value);

	void CacheMouseLocationOnViewport();
	void Rotate();

	void SelectionStarted();
	void SelectionCompleted();

	void UpdateSelectionDragState();

	void DoSingleSelect(const FVector2D& ScreenPosition);
	void DoBoxSelect(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB);




};
