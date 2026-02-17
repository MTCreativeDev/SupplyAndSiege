// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_UnitInformationComponent.generated.h"

class USAS_UnitManagerComponent;
class USAS_UnitTypeData;


//Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToggleSelectionRing, bool, bShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyTeamChange, ESAS_Team, NewTeam);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_UnitInformationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAS_UnitInformationComponent();

	UFUNCTION(BlueprintCallable)
	void SetTeam(ESAS_Team NewTeam);

	void RemoveUnitFromGame();

	void NotifySelected(ESAS_Team SelectedByTeam);

	void NotifyDeselected(ESAS_Team DeselectedByTeam);

	void IssueMoveOrder(FVector WorldLocation);

	//Dispatchers
	UPROPERTY(BlueprintAssignable)
	FToggleSelectionRing ToggleSelectionRing;

	UPROPERTY(BlueprintAssignable)
	FNotifyTeamChange NotifyTeamChange;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;



public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Information")
	ESAS_Team AssignedTeam = ESAS_Team::None;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit Information")
	USAS_UnitTypeData* UnitType = nullptr;
		
protected:

	UPROPERTY(BlueprintReadOnly)
	USAS_UnitManagerComponent* AssignedUnitManager;


};
