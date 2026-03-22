


#include "Widgets/SAS_BuildingWidget.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Misc/DataAssets/SAS_BuildingDefinitionData.h"

void USAS_BuildingWidget::NotifyControllerOfBuildingPlacement()
{
	if (!SASPlayerController) return;

	SASPlayerController->StartBuildingPlacement(BuildingDefinition);
}
