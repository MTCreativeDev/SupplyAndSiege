#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMousePosition : uint8
{
    None        UMETA(DisplayName = "None"),
    TopLeft     UMETA(DisplayName = "TopLeft"),
    Top         UMETA(DisplayName = "Top"),
    TopRight    UMETA(DisplayName = "TopRight"),
    Right       UMETA(DisplayName = "Right"),
    BottomRight UMETA(DisplayName = "BottomRight"),
    Bottom      UMETA(DisplayName = "Bottom"),
    BottomLeft  UMETA(DisplayName = "BottomLeft"),
    Left        UMETA(DisplayName = "Left")
};

//Enum used as a bitblocker to ensure we are allowed to move.
UENUM(BlueprintType, meta = (Bitflags))
enum class EMovementBlocker : uint8
{
    None = 0   UMETA(Hidden, displayName = "None"),
    Rotating = 1 << 0   UMETA(displayName = "Rotating"),
    Paused =  1 << 1    UMETA(displayName = "Paused")
};