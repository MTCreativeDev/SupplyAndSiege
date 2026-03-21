

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_SelectableWidget.generated.h"

class ASAS_PlayerController;

/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API USAS_SelectableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:

	UFUNCTION(BlueprintCallable, Category = "SAS_SelectableWidget")
	void NotifyControllerOfHover();

	UFUNCTION(BlueprintCallable, Category = "SAS_SelectableWidget")
	void NotifyControllerOfStopHover();


protected:

	UPROPERTY(BlueprintReadOnly, Category = "Player_Information")
	ASAS_PlayerController* SASPlayerController = nullptr;


};
