// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controllers/SAS_PlayerController.h"

EMousePosition ASAS_PlayerController::GetMouseEdgePosition(float HorizontalEdgeDistance, float VerticalEdgeDistance) const
{
    float MouseX, MouseY;
    if (!GetMousePosition(MouseX, MouseY))
    {
        return EMousePosition::None;
    }

    int32 ViewX, ViewY;
    GetViewportSize(ViewX, ViewY);

    const bool bLeft = MouseX <= HorizontalEdgeDistance;
    const bool bRight = MouseX >= ViewX - HorizontalEdgeDistance;
    const bool bTop = MouseY <= VerticalEdgeDistance;
    const bool bBottom = MouseY >= ViewY - VerticalEdgeDistance;

    // Corners first
    if (bTop && bLeft)     return EMousePosition::TopLeft;
    if (bTop && bRight)    return EMousePosition::TopRight;
    if (bBottom && bRight) return EMousePosition::BottomRight;
    if (bBottom && bLeft)  return EMousePosition::BottomLeft;

    // Edges
    if (bTop)    return EMousePosition::Top;
    if (bRight)  return EMousePosition::Right;
    if (bBottom) return EMousePosition::Bottom;
    if (bLeft)   return EMousePosition::Left;

    return EMousePosition::None;
}
