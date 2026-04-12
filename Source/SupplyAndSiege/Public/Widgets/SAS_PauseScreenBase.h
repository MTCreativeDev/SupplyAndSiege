// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_PauseScreenBase.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseScreenResumeClicked);

UCLASS()
class SUPPLYANDSIEGE_API USAS_PauseScreenBase : public UUserWidget
{
	GENERATED_BODY()
	
public: 
	UPROPERTY(BlueprintCallable) 
	FOnPauseScreenResumeClicked OnPauseScreenResumeClicked;
};
