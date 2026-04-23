#include "Widgets/SAS_LmQueueWidget.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_WorkerControlComponent.h"
#include "GameFramework/Actor.h"

void USAS_LmQueueWidget::SetSelectedUnits(const TArray<USAS_UnitInformationComponent*>& InSelectedUnits)
{
	SelectedUnits.Reset();
	ValidWorkerComponents.Reset();

	for (USAS_UnitInformationComponent* UnitInfo : InSelectedUnits)
	{
		if (!IsValid(UnitInfo)) continue;

		SelectedUnits.Add(UnitInfo);

		AActor* OwnerActor = UnitInfo->GetOwner();
		if (!IsValid(OwnerActor)) continue;

		USAS_WorkerControlComponent* WorkerControl = OwnerActor->FindComponentByClass<USAS_WorkerControlComponent>();
		if (!IsValid(WorkerControl)) continue;

		ValidWorkerComponents.AddUnique(WorkerControl);
	}
	RefreshWidget();
}

void USAS_LmQueueWidget::NativeConstruct()
{
	PlayerController = Cast<ASAS_PlayerController>(GetOwningPlayer());
	Super::NativeConstruct();
}

void USAS_LmQueueWidget::NativeDestruct()
{
	Super::NativeDestruct();
}




void USAS_LmQueueWidget::HandleAddToLMQueueClicked()
{
	if (!PlayerController) return;
	if (ValidWorkerComponents.Num() == 0) return;

	PlayerController->RequestAddWorkersToLMQueue(ValidWorkerComponents);




	RefreshWidget();
}

void USAS_LmQueueWidget::RefreshWidget()
{
	const bool bHasValidWorkers = ValidWorkerComponents.Num() > 0;
	AlreadyQueuedCount = 0;

	for (USAS_WorkerControlComponent* WorkerControl : ValidWorkerComponents)
	{
		if (!IsValid(WorkerControl)) continue;

		ESAS_WorkerControlState WorkerControlState = WorkerControl->GetCurrentWorkerControlState();
		if (WorkerControlState == ESAS_WorkerControlState::LMQueue || WorkerControlState == ESAS_WorkerControlState::Transition)
		{
			AlreadyQueuedCount++;
		}
	}

	BP_RefreshWidget(bHasValidWorkers, ValidWorkerComponents.Num(), AlreadyQueuedCount);
}

