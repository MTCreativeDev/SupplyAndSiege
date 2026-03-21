


#include "Widgets/SAS_SelectableWidget.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/SAS_Enumerators.h"

void USAS_SelectableWidget::NativeConstruct()
{
	SASPlayerController = Cast<ASAS_PlayerController>(GetOwningPlayer());
}

void USAS_SelectableWidget::NotifyControllerOfHover()
{
	SASPlayerController->AddSelectionInputBlocker(ESelectionBlocker::HoveringUI);
}

void USAS_SelectableWidget::NotifyControllerOfStopHover()
{
	SASPlayerController->RemoveSelectionInputBlocker(ESelectionBlocker::HoveringUI);
}
