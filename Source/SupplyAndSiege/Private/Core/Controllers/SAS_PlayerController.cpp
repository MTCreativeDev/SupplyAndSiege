

#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/Pawns/SAS_PlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Core/CustomCollision.h"
#include "Core/Components/SAS_UnitManagerComponent.h"
#include "Core/Objects/SAS_SelectionInventoryViewModel.h"
#include "Core/Actors/SAS_BL_BuildPlacement.h"
#include "Core/Actors/SAS_BL_BuildJob.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/SAS_PauseScreenBase.h"

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

    switch (CurrentSecondaryAction)
    {
    case ESecondaryControllerAction::None:
        break;
    case ESecondaryControllerAction::BuildingPlacement:
        {
            if (!BuildingPlacementActor) break;

            FVector NewWorldLocation;
            if (!GetNavigableLocationUnderMouse(NewWorldLocation)) break;

            FHitResult SweepHit;
            BuildingPlacementActor->SetActorLocation(NewWorldLocation, false, &SweepHit, ETeleportType::None);
            BuildingPlacementActor->CheckPlacementValidity();
            break;
        }
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
                
    }
}

void ASAS_PlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    PlayerPawn = Cast<ASAS_PlayerPawn>(InPawn);
}

void ASAS_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    InitializeSelectionInventoryViewModel();
}

void ASAS_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SelectionInventoryViewModel)
    {
        SelectionInventoryViewModel->Shutdown();
    }

    Super::EndPlay(EndPlayReason);
}

USAS_SelectionInventoryViewModel* ASAS_PlayerController::GetSelectionInventoryViewModel()
{
    if (!SelectionInventoryViewModel)
    {
        InitializeSelectionInventoryViewModel();
    }
    return SelectionInventoryViewModel;
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
    checkf(IA_PauseGame, TEXT("ASAS_PlayerController:IA_PauseGame, is not assigned. Set it in the PlayerController Blueprint defaults."));

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

    EIC->BindAction(IA_PauseGame, ETriggerEvent::Started, this, &ASAS_PlayerController::EscapeButtonPressed);
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
    if (IsSelectionInputBlocked()) return;

    switch(CurrentControllerSelectionMode)
    {
    case EControllerSelectionMode::Default:
    {
        bSelecting = true;
        bDragging = false;

        MovementBlockerMask |= static_cast<int64>(EMovementBlocker::Selecting);
        RotationBlockerMask |= static_cast<int64>(ERotationBlocker::Selecting);
        CurrentAction = EControllerAction::None;


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
        break;
    }

    case EControllerSelectionMode::BuildingPlacement:

        if (AttemptPlaceBuilding())
        {
            EndBuildingPlacement();
        }
        else
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Invalid"));
            }
        }


        break;

    default:
        break;
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

    MovementBlockerMask &= ~static_cast<int64>(EMovementBlocker::Selecting);
    RotationBlockerMask &= ~static_cast<int64>(ERotationBlocker::Selecting);

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
    CurrentRightClickAction = ERightClickAction::None;

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
        CurrentRightClickAction = ERightClickAction::UnitAction;

    }

    
}

void ASAS_PlayerController::DoBoxSelect(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB)
{
    if (!UnitManagerComponent) { return; }
    
    if (!IsInputKeyDown(EKeys::LeftShift) && !IsInputKeyDown(EKeys::RightShift))
    {
        UnitManagerComponent->ClearAllSelectedUnits();
        CurrentRightClickAction = ERightClickAction::None;
    }
    
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
        CurrentRightClickAction = ERightClickAction::UnitAction;
        //TODO: Have the right click action get updated after all of this rather than each time.
        //TODO: Set up a process to only have the unit manager broadcast an update after all of the selected units are complete. Right now this will cause the UI to update x amount of times based on units selected.
    }
}

void ASAS_PlayerController::RightClickStarted()
{
    switch (CurrentRightClickAction)
    {
    case ERightClickAction::None:
    {
        break;
    }
    case ERightClickAction::UnitAction:
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
            Trace_Interactable,
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
            UnitManagerComponent->RightClickReceived(Hit);
        }
        break;

    }

    default:
    {
        break;
    }

    }
}

void ASAS_PlayerController::RightClickCompleted()
{
}

void ASAS_PlayerController::EscapeButtonPressed()
{
    switch (CurrentEscapeAction)
    {
    case EEscapeAction::PauseGame:
        TogglePaused();
        break;

    case EEscapeAction::GoBack:
        OnGoBackPressed.Broadcast();

        if (CurrentSecondaryAction == ESecondaryControllerAction::BuildingPlacement)
        {

            EndBuildingPlacement();
        }

        break;

    default:
        break;
    }
}

void ASAS_PlayerController::TogglePaused()
{
    if (!bPaused)
    {
        if (UWorld* World = GetWorld())
        {
            UGameplayStatics::SetGamePaused(World, true);
            if (PauseScreenWidget)
            {
                PauseScreen = CreateWidget<USAS_PauseScreenBase>(World, PauseScreenWidget);
                
                if (PauseScreen)
                {
                    PauseScreen->AddToViewport(); 
                    
                    PauseScreen->OnPauseScreenResumeClicked.AddDynamic(this, &ASAS_PlayerController::TogglePaused);
                }
            }
        }
        
        MovementBlockerMask &= ~static_cast<int64>(EMovementBlocker::Rotating);
        MovementBlockerMask &= ~static_cast<int64>(EMovementBlocker::Selecting);
        RotationBlockerMask &= ~static_cast<int64>(ERotationBlocker::Moving);
        RotationBlockerMask &= ~static_cast<int64>(ERotationBlocker::Selecting);
        
        MovementBlockerMask |= static_cast<int64>(EMovementBlocker::Paused); 
        RotationBlockerMask |= static_cast<int64>(ERotationBlocker::Paused);
        
        bPaused = true;
    }
    else
    {
        if (PauseScreen)
        {
            PauseScreen->OnPauseScreenResumeClicked.RemoveAll(this);
            PauseScreen->RemoveFromParent(); 
        }
        
        if (UWorld* World = GetWorld())
        {
            UGameplayStatics::SetGamePaused(World, false);
        }
        
        MovementBlockerMask &= ~static_cast<int64>(EMovementBlocker::Paused);
        RotationBlockerMask &= ~static_cast<int64>(ERotationBlocker::Paused);
        
        bPaused = false;
    }
}

void ASAS_PlayerController::AddSelectionInputBlocker(ESelectionBlocker Blocker)
{
    SelectionBlockerMask |= (int32)Blocker;
}

void ASAS_PlayerController::RemoveSelectionInputBlocker(ESelectionBlocker Blocker)
{
    SelectionBlockerMask &= ~((int32)Blocker);
}

bool ASAS_PlayerController::IsSelectionInputBlocked() const
{
    return SelectionBlockerMask != 0;
}

void ASAS_PlayerController::StartBuildingPlacement(USAS_BuildingDefinitionData* BuildingToPlace)
{
    if (!BuildingToPlace) return;

    CurrentBuildingBeingPlaced = BuildingToPlace;

    SpawnPlacementActor();
    if (!BuildingPlacementActor) return;

    BuildingPlacementActor->SetBuildingDefinition(BuildingToPlace);
    BuildingPlacementActor->ApplyDefinitionToComponents();

    UnitManagerComponent->ClearAllSelectedUnits();
    CurrentControllerSelectionMode = EControllerSelectionMode::BuildingPlacement;

    SetSecondaryAction(ESecondaryControllerAction::BuildingPlacement);
}

void ASAS_PlayerController::EndBuildingPlacement()
{
    CurrentControllerSelectionMode = EControllerSelectionMode::Default;
    ClearSecondaryAction();
    
    DestroyBuildingPlacementActor();
    
    OnExitBuildingPlacement.Broadcast();
}

void ASAS_PlayerController::UpdateEscapeAction(EEscapeAction NewAction)
{
    CurrentEscapeAction = NewAction;
}

void ASAS_PlayerController::SetSecondaryAction(ESecondaryControllerAction NewAction)
{
    if (CurrentSecondaryAction != ESecondaryControllerAction::None) return;
    CurrentSecondaryAction = NewAction;
    return;
}

void ASAS_PlayerController::ClearSecondaryAction()
{
    CurrentSecondaryAction = ESecondaryControllerAction::None;
    return;
}

void ASAS_PlayerController::SpawnPlacementActor()
{
    if (BuildingPlacementActor) return;
    if (!GetWorld()) return;

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    BuildingPlacementActor = GetWorld()->SpawnActor<ASAS_BL_BuildPlacement>(
        BuildingPlacementActorClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        Params
        );
    return;
}

void ASAS_PlayerController::DestroyBuildingPlacementActor()
{
    if (BuildingPlacementActor && IsValid(BuildingPlacementActor))
    {

        BuildingPlacementActor->Destroy();
        BuildingPlacementActor = nullptr;
    }
}

bool ASAS_PlayerController::GetNavigableLocationUnderMouse(FVector& OutLocation)
{
    OutLocation = FVector::ZeroVector;

    FVector WorldOrigin;
    FVector WorldDirection;

    float MouseX, MouseY;
    if (!GetMousePosition(MouseX, MouseY)) return false;

    if (!DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection)) return false;

    const FVector TraceStart = WorldOrigin;
    const FVector TraceEnd = WorldOrigin + (WorldDirection * 100000.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.bTraceComplex = true;

    UWorld* World = GetWorld();
    if (!World) return false;

    const bool bHit = World->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        Trace_NavigableArea,
        Params
    );

    if (!bHit) return false;
    
    OutLocation = Hit.ImpactPoint;
    return true;
}

bool ASAS_PlayerController::AttemptPlaceBuilding()
{
    if (CurrentSecondaryAction != ESecondaryControllerAction::BuildingPlacement || !CurrentBuildingBeingPlaced || !BuildingPlacementActor) return false;

    if (!BuildingPlacementActor->GetIsValidPlacement()) return false;
    const FTransform NewBuildingTransform = BuildingPlacementActor->GetActorTransform();
    
    UWorld* World = GetWorld();
    if (!World) return false;

    FActorSpawnParameters Params;
    Params.Owner = nullptr;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ASAS_BL_BuildJob* NewBuildJob;

    NewBuildJob = World->SpawnActorDeferred<ASAS_BL_BuildJob>(
        BuildingUnderConstructionActorClass,
        NewBuildingTransform,
        nullptr,
        nullptr
        );

    NewBuildJob->SetBuildingDefinition(CurrentBuildingBeingPlaced);
    NewBuildJob->ApplyDefinitionToComponents();
    NewBuildJob->InitializeBuildJob(ESAS_Team::Team1);


    UGameplayStatics::FinishSpawningActor(NewBuildJob, NewBuildingTransform);
    return true;
}

void ASAS_PlayerController::InitializeSelectionInventoryViewModel()
{
    if (!UnitManagerComponent)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                5.f,
                FColor::Red,
                TEXT("Unit Manager Component must be initialized first")
            );
        }
        return;
    }
    if (!SelectionInventoryViewModel)
    {
        SelectionInventoryViewModel = NewObject<USAS_SelectionInventoryViewModel>(this);
        SelectionInventoryViewModel->Initialize(UnitManagerComponent);

    }
}

