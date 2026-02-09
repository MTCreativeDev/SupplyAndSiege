// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/Pawns/SAS_PlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Core/CustomCollision.h"
#include "Core/Components/SAS_UnitManagerComponent.h"

ASAS_PlayerController::ASAS_PlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    UnitManagerComponent = CreateDefaultSubobject<USAS_UnitManagerComponent>(TEXT("UnitManagerComponent"));
    //WARNING: This only works in a single player game.
    UnitManagerComponent->SetTeam(ESAS_Team::Team1);

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
        Rotate();
        break;

    case EControllerAction::None:
    default:
        break;
    }

    if (bSelecting)
    {
        float MouseX, MouseY;
        if (GetMousePosition(MouseX, MouseY))
        {
            CurrentSelectionMousePos = FVector2D(MouseX, MouseY);
        }

        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(
                    -1,
                    0.f,
                    FColor::Yellow,
                    FString::Printf(TEXT("Failed to get mouse pos"))
                );
            }
        }
        UpdateSelectionDragState();

        const FString StateString = bDragging ? TEXT("Dragging") : TEXT("Clicking");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                0.f,
                FColor::Cyan,
                StateString
            );
        }


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
    checkf(IA_Select, TEXT("ASAS_PlayerController: IA_Select is not assigned. Set it in the PlayerController Blueprint defaults."));
    checkf(IA_RightClick, TEXT("ASAS_PlayerController: IA_RightClick is not assigned. Set it in the PlayerController Blueprint defaults."));

    EIC->BindAction(IA_ToggleRotate, ETriggerEvent::Started, this, &ASAS_PlayerController::OnRotationToggleStarted);
    EIC->BindAction(IA_ToggleRotate, ETriggerEvent::Completed, this, &ASAS_PlayerController::OnRotationToggleEnded);
    EIC->BindAction(IA_ToggleRotate, ETriggerEvent::Canceled, this, &ASAS_PlayerController::OnRotationToggleEnded);

    EIC->BindAction(IA_Move, ETriggerEvent::Started, this, &ASAS_PlayerController::MoveUpdated);
    EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ASAS_PlayerController::MoveUpdated);
    EIC->BindAction(IA_Move, ETriggerEvent::Completed, this, &ASAS_PlayerController::MoveEnded);
    EIC->BindAction(IA_Move, ETriggerEvent::Canceled, this, &ASAS_PlayerController::MoveEnded);

    EIC->BindAction(IA_Select, ETriggerEvent::Started, this, &ASAS_PlayerController::SelectionStarted);
    EIC->BindAction(IA_Select, ETriggerEvent::Completed, this, &ASAS_PlayerController::SelectionCompleted);
    EIC->BindAction(IA_Select, ETriggerEvent::Canceled, this, &ASAS_PlayerController::SelectionCompleted);

    EIC->BindAction(IA_RightClick, ETriggerEvent::Started, this, &ASAS_PlayerController::RightClickStarted);
    EIC->BindAction(IA_RightClick, ETriggerEvent::Completed, this, &ASAS_PlayerController::RightClickCompleted);
    EIC->BindAction(IA_RightClick, ETriggerEvent::Canceled, this, &ASAS_PlayerController::RightClickCompleted);

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
    CacheMouseLocationOnViewport();
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

void ASAS_PlayerController::CacheMouseLocationOnViewport()
{
    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY))
    {
        CachedMouseLocation = FVector2D(MouseX, MouseY);
    }
    //If we can't get the mouse position on the viewport for some reason, set the cached location to the center of the screen.
    else
    {
        int32 ViewportX, ViewportY;
        GetViewportSize(ViewportX, ViewportY);
        CachedMouseLocation = FVector2D(ViewportX / 2, ViewportY / 2);
    }
}

void ASAS_PlayerController::Rotate()
{
    float MouseX, MouseY;
    if (!GetMousePosition(MouseX, MouseY))
    {
        return;
    }
    const float DeltaX = MouseX - CachedMouseLocation.X;
    if (MaxEvaluatedRotationDistance <= 0.f) return;
    const float Normalized = FMath::Clamp(DeltaX / MaxEvaluatedRotationDistance,-1.f,1.f);
    PlayerPawn->Rotate(Normalized);
}

void ASAS_PlayerController::SelectionStarted()
{
    bSelecting = true;
    bDragging = false;
    
    if (GetWorld())
    {
        SelectionStartedTime = GetWorld()->GetTimeSeconds();
    }
    else
    {
        SelectionStartedTime = 0.f;
    }
    
    float MouseX, MouseY;
    if (GetMousePosition(MouseX, MouseY))
    {
        SelectionStartMousePos = FVector2D(MouseX, MouseY);
        CurrentSelectionMousePos = SelectionStartMousePos;
    }
}

void ASAS_PlayerController::SelectionCompleted()
{
    if (!bSelecting) return;

    const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    const float HeldTime = CurrentTime - SelectionStartedTime;

    const float DragDist = FVector2D::Distance(CurrentSelectionMousePos, SelectionStartMousePos);
    const bool bWasDrag = bDragging || (DragDist >= SelectionDistanceTillDrag);
    const bool bWasClick = !bWasDrag && (HeldTime <= SelectingTimeTillDrag);

    if (bWasClick)
    {
        DoSingleSelect(SelectionStartMousePos);
    }
    else
    {
        DoBoxSelect(SelectionStartMousePos, CurrentSelectionMousePos);
    }

    bSelecting = false;
    bDragging = false;
}

void ASAS_PlayerController::UpdateSelectionDragState()
{
    if (bDragging)
    {
        return;
    }

    const float DragDist = FVector2D::Distance(CurrentSelectionMousePos, SelectionStartMousePos);
    if (DragDist >= SelectionDistanceTillDrag)
    {
        bDragging = true;
    }
}

void ASAS_PlayerController::DoSingleSelect(const FVector2D& ScreenPosition)
{
    //TODO: Set up the logic to check if the player is trying to select additional units instead of only select. E.g. holding shift or something like that.
    if (!UnitManagerComponent) return;
    UnitManagerComponent->ClearAllSelectedUnits();

    FVector WorldOrigin;
    FVector WorldDirection;

    if (!DeprojectScreenPositionToWorld(SelectionStartMousePos.X, SelectionStartMousePos.Y, WorldOrigin, WorldDirection)) return;

    const FVector TraceStart = WorldOrigin;
    const FVector TraceEnd = WorldOrigin + (WorldDirection * 100000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = true;
    
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        Trace_SelectableUnit,
        Params
    );

    if (bHit && Hit.GetActor())
    {
        const AActor* Actor = Hit.GetActor();

        USAS_UnitInformationComponent* UnitInformationComponent = Actor->FindComponentByClass<USAS_UnitInformationComponent>();
        if (!UnitInformationComponent) return;
    
        UnitManagerComponent->AddSelectedUnit(UnitInformationComponent);

    }

    
}

void ASAS_PlayerController::DoBoxSelect(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB)
{
    if (!UnitManagerComponent) return;
    UnitManagerComponent->ClearAllSelectedUnits();
    
    const float MinX = FMath::Min(SelectionStartMousePos.X, CurrentSelectionMousePos.X);
    const float MinY = FMath::Min(SelectionStartMousePos.Y, CurrentSelectionMousePos.Y);
    const float MaxX = FMath::Max(SelectionStartMousePos.X, CurrentSelectionMousePos.X);
    const float MaxY = FMath::Max(SelectionStartMousePos.Y, CurrentSelectionMousePos.Y);

    for (const TWeakObjectPtr<AActor>& WeakActor : UnitManagerComponent->SelectableUnits)
    {
        if (!WeakActor.IsValid()) continue;
        AActor* Actor = WeakActor.Get();

        FVector2D ScreenPos;
        if (!ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenPos, false)) continue;

        const bool bInside = ScreenPos.X >= MinX && ScreenPos.X <= MaxX && ScreenPos.Y >= MinY && ScreenPos.Y <= MaxY;

        if (!bInside) continue;

        USAS_UnitInformationComponent* InfoComponent = Actor->FindComponentByClass<USAS_UnitInformationComponent>();
        if (!InfoComponent) continue;

        UnitManagerComponent->AddSelectedUnit(InfoComponent);
    }
}

void ASAS_PlayerController::RightClickStarted()
{

    float MouseX, MouseY;
    if (!GetMousePosition(MouseX, MouseY)) return;

    FVector WorldOrigin;
    FVector WorldDirection;

    if (!DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection)) return;

    const FVector TraceStart = WorldOrigin;
    const FVector TraceEnd = WorldOrigin + (WorldDirection * 100000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = true;

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        Trace_NavigableArea,
        Params
    );

    //Debug line trace to show where we clicked
    DrawDebugLine(
        GetWorld(),
        TraceStart,
        TraceEnd,
        FColor::Blue,
        false,
        2.f,
        0,
        .1
    );

    if (bHit && Hit.GetActor())
    {
        const AActor* Actor = Hit.GetActor();

        //debug print
        if (Actor)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.f,
                FColor::Green,
                FString::Printf(TEXT("Hit Actor: %s"), *Actor->GetName())
            );
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.f,
                FColor::Red,
                TEXT("Hit Actor: NONE")
            );

        }   
        //end debug print

    }
}

void ASAS_PlayerController::RightClickCompleted()
{
}

