

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SAS_PlayerVisibilityInterface.generated.h"

UINTERFACE(BlueprintType)
class USAS_PlayerVisibilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SUPPLYANDSIEGE_API ISAS_PlayerVisibilityInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit Visibility")
	void SetVisibleToPlayer(bool bVisible);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit Visibility")
	bool IsVisibleToPlayer() const;

};
