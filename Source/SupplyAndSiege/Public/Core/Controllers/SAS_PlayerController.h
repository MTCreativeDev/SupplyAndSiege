// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_PlayerController.generated.h"

class USAS_PauseScreenBase;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ASAS_PlayerPawn;
class USAS_UnitManagerComponent;
class USAS_SelectionInventoryViewModel;
class USAS_BuildingDefinitionData;
class ASAS_BL_BuildPlacement;
class ASAS_BL_BuildJob;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitBuildingPlacement);

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

	ASAS_PlayerController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory Selection View")
	USAS_SelectionInventoryViewModel* GetSelectionInventoryViewModel();

	void AddSelectionInputBlocker(ESelectionBlocker Blocker);
	void RemoveSelectionInputBlocker(ESelectionBlocker Blocker);
	bool IsSelectionInputBlocked() const;

	void StartBuildingPlacement(USAS_BuildingDefinitionData* BuildingToPlace);
	void EndBuildingPlacement();

	//Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Building_Placement")
	FExitBuildingPlacement OnExitBuildingPlacement;

protected:

	virtual void SetupInputComponent() override;

	FMouseEdgeResult GetMouseEdgePosition(float HorizontalEdgeDistance, float VerticalEdgeDistance) const;

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

	void RightClickStarted();
	void RightClickCompleted();

	UFUNCTION(BlueprintCallable) 
	void TogglePaused();

private:
	
	void InitializeSelectionInventoryViewModel();

	void SetSecondaryAction(ESecondaryControllerAction NewAction);
	void ClearSecondaryAction();

	void SpawnPlacementActor();
	void DestroyBuildingPlacementActor();

	bool GetNavigableLocationUnderMouse(FVector& OutLocation);

	bool AttemptPlaceBuilding();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (Bitmask, BitmaskEnum = "/Script/SUPPLYANDSIEGE.EMovementBlocker"))
	int32 MovementBlockerMask = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (Bitmask, BitmaskEnum = "/Script/SUPPLYANDSIEGE.ERotationBlocker"))
	int32 RotationBlockerMask = 0;

	UPROPERTY(BlueprintReadOnly)
	FVector2D SelectionStartMousePos = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector2D CurrentSelectionMousePos = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	bool bDragging = false;

	UPROPERTY(BlueprintReadWrite)
	bool bPaused = false; 
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (Bitmask, BitmaskEnum = "/Script/SUPPLYANDSIEGE.ESelectionBlocker"))
	int32 SelectionBlockerMask = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USAS_UnitManagerComponent* UnitManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller_State")
	EControllerAction CurrentAction = EControllerAction::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller_State")
	ESecondaryControllerAction CurrentSecondaryAction = ESecondaryControllerAction::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller_State")
	EControllerSelectionMode CurrentControllerSelectionMode = EControllerSelectionMode::Default;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_PlayerMovement;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_ToggleRotate;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Select;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_RightClick;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_PauseGame;

	FMouseEdgeResult MouseEdgeResult = FMouseEdgeResult();

	UPROPERTY(EditDefaultsOnly,Category = "Input")
	FVector2D ScreenMovementBounds = FVector2D(20.f, 20.f);

	bool OverrideMoveByInputAction = false;

	FVector2D MoveInputActionAxis = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	ASAS_PlayerPawn* PlayerPawn;

	FVector2D CachedMouseLocation = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float MaxEvaluatedRotationDistance = 400.f;

	// Selection States - I'm using two bools instead of an Enum since I don't expect there to be more than the two states.
	bool bSelecting = false;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float SelectingTimeTillDrag = 9.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float SelectionDistanceTillDrag = 8.f;

	float SelectionStartedTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Selected_Inventory")
	USAS_SelectionInventoryViewModel* SelectionInventoryViewModel;

	UPROPERTY(BlueprintReadOnly, Category = "Building_Placement")
	USAS_BuildingDefinitionData* CurrentBuildingBeingPlaced = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Building_Placement")
	ASAS_BL_BuildPlacement* BuildingPlacementActor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Placement")
	TSubclassOf<ASAS_BL_BuildPlacement> BuildingPlacementActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building_Placement")
	TSubclassOf<ASAS_BL_BuildJob> BuildingUnderConstructionActorClass;
	
	UPROPERTY(EditDefaultsOnly) 
	TSubclassOf<UUserWidget> PauseScreenWidget;
	
	UPROPERTY() 
	USAS_PauseScreenBase* PauseScreen;
};
