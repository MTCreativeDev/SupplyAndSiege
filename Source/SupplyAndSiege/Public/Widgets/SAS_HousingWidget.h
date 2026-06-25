

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_HousingWidget.generated.h"


class USAS_PopulationManagerComponent;

UCLASS(Blueprintable)
class SUPPLYANDSIEGE_API USAS_HousingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Housing")
	USAS_PopulationManagerComponent* PopulationManagerComponent;
	
};
