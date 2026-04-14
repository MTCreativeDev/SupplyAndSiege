

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_LogisticsDebugWidget.generated.h"

class USAS_LogisticsManagerComponent;

UCLASS(Blueprintable)
class SUPPLYANDSIEGE_API USAS_LogisticsDebugWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Logistics")
	USAS_LogisticsManagerComponent* LogisticsManagerComponent = nullptr;
	
};
