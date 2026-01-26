// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controllers/SAS_PlayerController.h"

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
