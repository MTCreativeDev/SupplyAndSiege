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

//Enum used as a bitblocker to ensure we are allowed to rotate.
UENUM(BlueprintType, meta = (Bitflags))
enum class ERotationBlocker : uint8
{
    None = 0   UMETA(Hidden, displayName = "None"),
    Moving = 1 << 0   UMETA(displayName = "Moving"),
    Paused = 1 << 1   UMETA(displayName = "Paused"),
};

UENUM(BlueprintType)
enum class EControllerAction : uint8
{
    None    UMETA(DisplayName = "None"),
    Moving    UMETA(DisplayName = "Moving"),
    Rotating    UMETA(DisplayName = "Rotating")
};

UENUM(BlueprintType)
enum class ESAS_Team : uint8
{
    None    UMETA(DisplayName = "None"),
    EnvironmentTeam    UMETA(DisplayName = "Environment Team"),
    Team1    UMETA(DisplayName = "Team 1"),
    Team2    UMETA(DisplayName = "Team 2")
};