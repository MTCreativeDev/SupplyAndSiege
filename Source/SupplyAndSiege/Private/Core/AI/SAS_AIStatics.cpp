// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AI/SAS_AIStatics.h"

#include "Core/Components/SAS_UnitManagerComponent.h"

void USAS_AIStatics::AssignSelectableUnit(USAS_UnitManagerComponent* UnitManagerComp, AActor* NewUnit)
{
	UnitManagerComp->AssignSelectableUnit(NewUnit, true);
}

TArray<AActor*> USAS_AIStatics::GetSelectableUnits(USAS_UnitManagerComponent* UnitManagerComp) 
{
	TArray<AActor*> Result;
	Result.Reserve(UnitManagerComp->SelectableUnits.Num());
	
	for (const TWeakObjectPtr<AActor>& Unit : UnitManagerComp->SelectableUnits)
	{
		if (AActor* Actor = Unit.Get())
		{
			Result.Add(Actor);
		}
	}
	
	return Result;
}

void USAS_AIStatics::RightClickReceived(USAS_UnitManagerComponent* UnitManagerComp, FVector WorldLocation) 
{
	UnitManagerComp->RightClickReceived(WorldLocation);
}

void USAS_AIStatics::AddSelectedUnit(USAS_UnitManagerComponent* UnitManagerComp, AActor* Unit) 
{
	USAS_UnitInformationComponent* UnitInfoComp = Unit->FindComponentByClass<USAS_UnitInformationComponent>();
	UnitManagerComp->AddSelectedUnit(UnitInfoComp);
}
