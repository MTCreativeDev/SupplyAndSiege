

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SAS_SelectableWidget.h"
#include "SAS_BuildingWidget.generated.h"

class USAS_BuildingDefinitionData;

UCLASS()
class SUPPLYANDSIEGE_API USAS_BuildingWidget : public USAS_SelectableWidget
{
	GENERATED_BODY()


protected:
	UFUNCTION(BlueprintCallable, Category = "Build Widget")
	void NotifyControllerOfBuildingPlacement();


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Build Widget", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<USAS_BuildingDefinitionData> BuildingDefinition;
	
};
