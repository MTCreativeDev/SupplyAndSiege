// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SAS_PlayerScreenWidget.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/Components/SAS_UnitManagerComponent.h"


void UPlayerScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HOOK_LOG_QUIET();
	PlayerController = Cast<ASAS_PlayerController>(GetOwningPlayer());
	if (!PlayerController) return;

	UnitManagerComponent = PlayerController->FindComponentByClass<USAS_UnitManagerComponent>();
	if (UnitManagerComponent)
	{
		UnitManagerComponent->OnUnitSelectionChange.AddDynamic(this, &UPlayerScreenWidget::HandleSelectedUnitsChanged);
	}

	InventoryViewModel = PlayerController->GetSelectionInventoryViewModel();
	if (!InventoryViewModel) return;

	InventoryViewModel->OnSelectionInventoryModelChanged.AddDynamic(this, &UPlayerScreenWidget::HandleSelectionInventoryModelChanged);
	HandleSelectionInventoryModelChanged();
}

void UPlayerScreenWidget::NativeDestruct()
{
	if (UnitManagerComponent)
	{
		UnitManagerComponent->OnUnitSelectionChange.RemoveDynamic(this, &UPlayerScreenWidget::HandleSelectedUnitsChanged);
	}

	if (InventoryViewModel)
	{
		InventoryViewModel->OnSelectionInventoryModelChanged.RemoveDynamic(this, &UPlayerScreenWidget::HandleSelectionInventoryModelChanged);
	}


	Super::NativeDestruct();


}

void UPlayerScreenWidget::HandleSelectedUnitsChanged(const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>& NewSelection)
{
	TArray<USAS_UnitInformationComponent*> BPArray;
	BPArray.Reserve(NewSelection.Num());

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& Ptr : NewSelection)
	{
		if (Ptr.IsValid())
		{
			BPArray.Add(Ptr.Get());
		}
	}

	BP_SelectedUnitsChanged(BPArray);
}


void UPlayerScreenWidget::BP_SelectedUnitsChanged_Implementation(const TArray<USAS_UnitInformationComponent*>& NewSelection)
{
	//handled in BP
}

void UPlayerScreenWidget::HandleSelectionInventoryModelChanged()
{
	if (!InventoryViewModel) return;

	const ESAS_SelectionInventoryMode Mode = InventoryViewModel->GetMode();

	const TMap<FPrimaryAssetId, int32>& Totals = InventoryViewModel->GetGroupTotals();

	BP_OnSelectionInventoryChanged(Mode, Totals);
}

void UPlayerScreenWidget::BP_OnSelectionInventoryChanged_Implementation(ESAS_SelectionInventoryMode Mode, const TMap<FPrimaryAssetId, int32>& Totals)
{
	//handled in BP
}
