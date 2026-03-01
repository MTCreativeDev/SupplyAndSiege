#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SAS_ClickTarget.generated.h"


//This enum will live here as anything referencing it should be aware of this interface.
UENUM(BlueprintType)
enum class ESAS_ClickTargetType : uint8
{
    Ground       UMETA(DisplayName = "Ground"),
    Unit         UMETA(DisplayName = "Unit"),
    Resource     UMETA(DisplayName = "Resource")
};

UINTERFACE(BlueprintType)
class USAS_ClickTarget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SUPPLYANDSIEGE_API ISAS_ClickTarget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    ESAS_ClickTargetType GetClickTargetType() const;
};
