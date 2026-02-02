// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_UnitManagerComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"

// Sets default values for this component's properties
USAS_UnitManagerComponent::USAS_UnitManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_UnitManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USAS_UnitManagerComponent::AssignSelectableUnit(TWeakObjectPtr<AActor> NewUnit, bool BypassComponentCheck)
{
    if (!BypassComponentCheck)
    {
        //Only accepts units that have the UnitInformationComponent. The bool input is used if the component is the thing adding the unit.
        if (!NewUnit.IsValid()) return;

        AActor* Actor = NewUnit.Get();
        USAS_UnitInformationComponent* UnitInformationComponent = Actor->FindComponentByClass<USAS_UnitInformationComponent>();
        if (!UnitInformationComponent) return;
    }
    //Only accepts units that have the UnitInformationComponent
    SelectableUnits.AddUnique(NewUnit);       
}

void USAS_UnitManagerComponent::RemoveSelectableUnit(TWeakObjectPtr<AActor> UnitToRemove)
{
    SelectableUnits.Remove(UnitToRemove);
    //TODO:: Need to set it up so that when a unit is removed it is removed from any current unit selections.
}
