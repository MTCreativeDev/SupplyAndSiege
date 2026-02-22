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


}

void UPlayerScreenWidget::NativeDestruct()
{
	if (UnitManagerComponent)
	{
		UnitManagerComponent->OnUnitSelectionChange.RemoveDynamic(this, &UPlayerScreenWidget::HandleSelectedUnitsChanged);
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
