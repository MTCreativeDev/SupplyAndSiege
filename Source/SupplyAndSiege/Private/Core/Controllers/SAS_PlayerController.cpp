// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controllers/SAS_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Core/Pawns/SAS_PlayerPawn.h"



ASAS_PlayerController::ASAS_PlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASAS_PlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (MovementBlockerMask == 0)
    {
        MouseEdgeResult = GetMouseEdgePosition(ScreenMovementBounds.X, ScreenMovementBounds.Y);
        if (!(MouseEdgeResult.MousePosition == EMousePosition::None))
        {
            CurrentAction = EControllerAction::Moving;
            RotationBlockerMask |= static_cast<int64>(ERotationBlocker::Moving);
        }
        else
        {
            if (CurrentAction == EControllerAction::Moving && OverrideMoveByInputAction == false)
            {
                CurrentAction = EControllerAction::None;
                RotationBlockerMask &= ~static_cast<int64>(ERotationBlocker::Moving);
            }
        }
    }
    switch (CurrentAction)
    {
    case EControllerAction::Moving:
        Move();
        break;

    case EControllerAction::Rotating:
        break;

    case EControllerAction::None:
    default:
        break;
    }
}

void ASAS_PlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    PlayerPawn = Cast<ASAS_PlayerPawn>(InPawn);
}

void ASAS_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);

    checkf(EIC, TEXT("ASAS_PlayerController requires Enhanced Input. InputComponent is not UEnhancedInputComponent."));
    checkf(IA_ToggleRotate, TEXT("ASAS_PlayerController: IA_Rotate is not assigned. Set it in the PlayerController Blueprint defaults."));
    checkf(IA_Move, TEXT("ASAS_PlayerController: IA_Move is not assigned. Set it in the PlayerController Blueprint defaults."));

    EIC->BindAction(IA_ToggleRotate, ETriggerEvent::Started, this, &ASAS_PlayerController::OnRotationToggleStarted);
    EIC->BindAction(IA_ToggleRotate, ETriggerEvent::Completed, this, &ASAS_PlayerController::OnRotationToggleEnded);
    EIC->BindAction(IA_ToggleRotate, ETriggerEvent::Canceled, this, &ASAS_PlayerController::OnRotationToggleEnded);

    EIC->BindAction(IA_Move, ETriggerEvent::Started, this, &ASAS_PlayerController::MoveUpdated);
    EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASAS_PlayerController::MoveUpdated);
    EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &ASAS_PlayerController::MoveEnded);
    EIC->BindAction(IA_Move, ETriggerEvent::Canceled, this, &ASAS_PlayerController::MoveEnded);
}

void ASAS_PlayerController::MoveUpdated(const FInputActionValue& Value)
{
    if (MovementBlockerMask != 0) return;
    OverrideMoveByInputAction = true;
    MoveInputActionAxis = Value.Get<FVector2D>();
    RotationBlockerMask |= static_cast<int64>(ERotationBlocker::Moving);
    CurrentAction = EControllerAction::Moving;

}

void ASAS_PlayerController::MoveEnded(const FInputActionValue& Value)
{
    (void)Value;
    OverrideMoveByInputAction = false;
    MoveInputActionAxis = FVector2D::ZeroVector;
    if (CurrentAction == EControllerAction::Moving && MouseEdgeResult.MousePosition == EMousePosition::None)
    {
        CurrentAction = EControllerAction::None;
        RotationBlockerMask &= ~static_cast<int64>(ERotationBlocker::Moving);
    }
}


FMouseEdgeResult ASAS_PlayerController::GetMouseEdgePosition(float HorizontalEdgeDistance, float VerticalEdgeDistance) const
{
    float MouseX;
    float MouseY;

    if (!GetMousePosition(MouseX, MouseY))
    {
        return FMouseEdgeResult();
    }


    int32 ViewX, ViewY;
    GetViewportSize(ViewX, ViewY);


    if(HorizontalEdgeDistance > ViewX * 0.5f || VerticalEdgeDistance > ViewY * 0.5f)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("The viewport is too small. Edge checks will force the screen to always move."));
        return FMouseEdgeResult();
    }

    //Determine if the mouse is in range of any of the edges of the screen.
    const bool bLeft = MouseX <= HorizontalEdgeDistance;
    const bool bRight = MouseX >= ViewX - HorizontalEdgeDistance;
    const bool bTop = MouseY <= VerticalEdgeDistance;
    const bool bBottom = MouseY >= ViewY - VerticalEdgeDistance;

    //If the mouse isnt near any edges, eject
    if (!bLeft && !bRight && !bTop && !bBottom)
    {
        return FMouseEdgeResult();
    }

    //Next, determine how far into the edge of the screen the mouse is. The further the player moves towards the edge, the faster we should move. 
    float fHorizontalMovement = 0.f;
    float fVerticalMovement = 0.f;

    if (bTop)
    {
        fVerticalMovement = 1.f - (MouseY / VerticalEdgeDistance);
    }
    if (bRight)
    {
        fHorizontalMovement = (MouseX - (ViewX - HorizontalEdgeDistance)) / HorizontalEdgeDistance;
    }
    if (bLeft)
    {
        fHorizontalMovement = -(1.f - (MouseX / HorizontalEdgeDistance));
    }
    if (bBottom)
    {
        fVerticalMovement = -((MouseY - (ViewY - VerticalEdgeDistance)) / VerticalEdgeDistance);
    }

    // Next set the correct MousePosition value

    EMousePosition MousePosition = EMousePosition::None;

    if (bTop && bLeft)     MousePosition = EMousePosition::TopLeft;
    if (bTop && bRight)    MousePosition = EMousePosition::TopRight;
    if (bBottom && bRight) MousePosition = EMousePosition::BottomRight;
    if (bBottom && bLeft)  MousePosition = EMousePosition::BottomLeft;

    if (MousePosition == EMousePosition::None)
    {
        if (bTop)    MousePosition = EMousePosition::Top;
        if (bRight)  MousePosition = EMousePosition::Right;
        if (bBottom) MousePosition = EMousePosition::Bottom;
        if (bLeft)   MousePosition = EMousePosition::Left;
    }

    FMouseEdgeResult Result;
    Result.MousePosition = MousePosition;
    Result.MovementAxis = FVector2D(fHorizontalMovement, fVerticalMovement);

    return Result;
}

void ASAS_PlayerController::OnRotationToggleStarted(const FInputActionValue& Value)
{
    //mark the input as irrelevant
    (void)Value;
    
    if (RotationBlockerMask != 0) return;

    MovementBlockerMask |= static_cast<int64>(EMovementBlocker::Rotating);
    CurrentAction = EControllerAction::Rotating;
}

void ASAS_PlayerController::OnRotationToggleEnded(const FInputActionValue& Value)
{
    //mark the input as irrelevant
    (void)Value;
    MovementBlockerMask &= ~static_cast<int64>(EMovementBlocker::Rotating);
    if (CurrentAction == EControllerAction::Rotating)
    {
        CurrentAction = EControllerAction::None;
    }
}

void ASAS_PlayerController::Move()
{
    if (!PlayerPawn) return;

    //First portion that handles movement via the mouse position.
    if(!OverrideMoveByInputAction)
    {
        PlayerPawn->Move(MouseEdgeResult.MovementAxis);
    }
    else 
    {
        PlayerPawn->Move(MoveInputActionAxis);
    }
}
