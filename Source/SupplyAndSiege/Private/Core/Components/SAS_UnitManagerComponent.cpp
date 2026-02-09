// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_UnitManagerComponent.h"


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
    if (!NewUnit.IsValid()) return;

    USAS_UnitInformationComponent* UnitInformationComponent = nullptr;

    if (!BypassComponentCheck)
    {
        //Only accepts units that have the UnitInformationComponent. The bool input is used if the component is the thing adding the unit.
        AActor* Actor = NewUnit.Get();
        UnitInformationComponent = Actor->FindComponentByClass<USAS_UnitInformationComponent>();
        if (!UnitInformationComponent) return;
    }

    //Only accepts units that have the UnitInformationComponent
    SelectableUnits.AddUnique(NewUnit);       



}

void USAS_UnitManagerComponent::SetTeam(ESAS_Team NewTeam)
{
    AssignedTeam = NewTeam;
}

void USAS_UnitManagerComponent::RemoveSelectableUnit(TWeakObjectPtr<AActor> UnitToRemove)
{
    SelectableUnits.Remove(UnitToRemove);

    //TODO:: Need to set it up so that when a unit is removed it is removed from any current unit selections.


     //Debug print
    if (GEngine)
    {
        FString ActorList;

        for (const TWeakObjectPtr<AActor>& Unit : SelectableUnits)
        {
            if (Unit.IsValid())
            {
                ActorList += Unit->GetName();
                ActorList += TEXT(", ");
            }
        }

        GEngine->AddOnScreenDebugMessage(
            -1,
            30.f,
            FColor::Green,
            FString::Printf(
                TEXT("SelectableUnits Count: %d | [%s]"),
                SelectableUnits.Num(),
                *ActorList
            )
        );
    }
}

void USAS_UnitManagerComponent::AddSelectedUnit(TWeakObjectPtr<USAS_UnitInformationComponent> UnitInformation)
{
    if (!UnitInformation.IsValid()) return;
    SelectedUnits.AddUnique(UnitInformation);

    UnitInformation->NotifySelected(AssignedTeam);

}

void USAS_UnitManagerComponent::RemoveSelectedUnit(TWeakObjectPtr<USAS_UnitInformationComponent> UnitInformation)
{
    SelectedUnits.Remove(UnitInformation);
    UnitInformation->NotifyDeselected(AssignedTeam);
}

void USAS_UnitManagerComponent::ClearAllSelectedUnits()
{
    for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitCompPtr : SelectedUnits)
    {
        if (!UnitCompPtr.IsValid()) continue;
        UnitCompPtr->NotifyDeselected(AssignedTeam);
    }
    SelectedUnits.Empty();
}

void USAS_UnitManagerComponent::IssueMoveOrderToSelectedUnits(FVector WorldLocation)
{
    for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitCompPtr : SelectedUnits)
    {
        if (!UnitCompPtr.IsValid()) continue;
        //TODO. Need to set up a proper formation creation process so that multiple actors dont get assigned the same exact location.
        UnitCompPtr->IssueMoveOrder(WorldLocation);
    }
}

void USAS_UnitManagerComponent::RightClickReceived(FVector WorldLocation)
{
    //TODO: Need to update this function to account for various possible right clicks. E.g. attack htis actor, harvest this unit  or move here etc.
    if (SelectedUnits.Num() == 0) return;

    //TODO: set up  logic to check if the selected units can even move.

    IssueMoveOrderToSelectedUnits(WorldLocation);

}
