// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SAS_PlayerScreenWidget.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/Components/SAS_UnitManagerComponent.h"


void UPlayerScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
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

	if (Mode == ESAS_SelectionInventoryMode::None)
	{
		//TODO: Remove all shown inventory
		return;
	}

	const TMap<FPrimaryAssetId, int32>& Totals = InventoryViewModel->GetGroupTotals();

	for (const TPair<FPrimaryAssetId, int32>& Pair : Totals)
	{
		const FString Msg = FString::Printf(
			TEXT("Item: %s | Qty: %d"),
			*Pair.Key.ToString(),
			Pair.Value
		);

		UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
		}
	}

	//TODO: Pass this informaton along to the widget.

}
