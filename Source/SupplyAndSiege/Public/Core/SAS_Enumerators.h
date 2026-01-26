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
