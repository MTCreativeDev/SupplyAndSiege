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
    Paused =  1 << 1    UMETA(displayName = "Paused"),
    Selecting = 1 << 2    UMETA(displayName = "Selecting")
};

//Enum used as a bitblocker to ensure we are allowed to rotate.
UENUM(BlueprintType, meta = (Bitflags))
enum class ERotationBlocker : uint8
{
    None = 0   UMETA(Hidden, displayName = "None"),
    Moving = 1 << 0   UMETA(displayName = "Moving"),
    Paused = 1 << 1   UMETA(displayName = "Paused"),
    Selecting = 1 << 2    UMETA(displayName = "Selecting")
};

//Enum used as a bitblocker to ensure we are allowed to select.
UENUM(BlueprintType, meta = (Bitflags))
enum class ESelectionBlocker : uint8
{
    None = 0   UMETA(Hidden, displayName = "None"),
    HoveringUI = 1 << 0   UMETA(displayName = "Hovering UI"),
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
enum class ESecondaryControllerAction : uint8
{
    None    UMETA(DisplayName = "None"),
    BuildingPlacement    UMETA(DisplayName = "Building Placement")
};

UENUM(BlueprintType)
enum class EControllerSelectionMode : uint8
{
    Default    UMETA(DisplayName = "Default"),
    BuildingPlacement    UMETA(DisplayName = "Building Placement")
};

UENUM(BlueprintType)
enum class ESAS_Team : uint8
{
    None    UMETA(DisplayName = "None"),
    EnvironmentTeam    UMETA(DisplayName = "Environment Team"),
    Team1    UMETA(DisplayName = "Team 1"),
    Team2    UMETA(DisplayName = "Team 2")
};

UENUM(BlueprintType)
enum class ESAS_TrackedInventoryCategory : uint8
{
    None    UMETA(DisplayName = "None"),
    Stockpile    UMETA(DisplayName = "Stockpile"),
    Transit    UMETA(DisplayName = "Transit"),
    RefinerInput    UMETA(DisplayName = "Refiner Input")
};

UENUM(BlueprintType)
enum class ESAS_SelectionInventoryMode : uint8
{
    None    UMETA(DisplayName = "None"),
    Single  UMETA(DisplayName = "Single"),
    Group   UMETA(DisplayName = "Group")
};

UENUM(BlueprintType)
enum class ESAS_ResourceValidity : uint8
{
    Valid   UMETA(DisplayName = "Valid"),
    InvalidKey   UMETA(DisplayName = "Invalid Key"),
    Depleted   UMETA(DisplayName = "Depleted"),
    ReservedByOther   UMETA(DisplayName = "Reserved by Other"),
};

UENUM(BlueprintType)
enum class ESAS_UnitCategory : uint8
{
    None   UMETA(DisplayName = "None"),
    Pawn_Villager   UMETA(DisplayName = "Villager"),
    Pawn_Courier   UMETA(DisplayName = "Courier"),
    Pawn_Military   UMETA(DisplayName = "Military"),
};

UENUM(BlueprintType)
enum class ESAS_UnitOrderType : uint8
{
    None   UMETA(DisplayName = "None"),
    Harvest   UMETA(DisplayName = "Harvest")
};