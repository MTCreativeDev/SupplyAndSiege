// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_UnitManagerComponent.h"


USAS_UnitInformationComponent::USAS_UnitInformationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USAS_UnitInformationComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USAS_UnitInformationComponent::SetTeam(ESAS_Team NewTeam)
{
	if (AssignedUnitManager && AssignedTeam != NewTeam)
	{
		AActor* Owner = GetOwner();
		if (!Owner) return;
		AssignedUnitManager->RemoveSelectableUnit(Owner);

		/*
			START HERE	
			switch(ESAS_Team)

		
		*/

	}
}


