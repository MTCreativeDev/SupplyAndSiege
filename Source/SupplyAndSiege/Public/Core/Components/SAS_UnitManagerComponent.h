// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "SAS_UnitManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_UnitManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAS_UnitManagerComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESAS_Team AssignedTeam = ESAS_Team::None;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	//This is the array of all available units to this player. Units include buildings, pawns etc. Anything that the player can select.
	TArray<TWeakObjectPtr<AActor>> SelectableUnits;

	TArray<TWeakObjectPtr<USAS_UnitInformationComponent>> SelectedUnits;

	void AssignSelectableUnit(TWeakObjectPtr<AActor> NewUnit, bool BypassComponentCheck);

	void RemoveSelectableUnit(TWeakObjectPtr<AActor> UnitToRemove);

	void AddSelectedUnit(TWeakObjectPtr<USAS_UnitInformationComponent> UnitInformation);

	void RemoveSelectedUnit(TWeakObjectPtr<USAS_UnitInformationComponent> UnitInformation);

	void ClearAllSelectedUnits();




protected:


};
