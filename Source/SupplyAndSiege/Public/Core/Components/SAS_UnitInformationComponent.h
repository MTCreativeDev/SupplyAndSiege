// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_UnitInformationComponent.generated.h"

class USAS_UnitManagerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_UnitInformationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAS_UnitInformationComponent();

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESAS_Team AssignedTeam = ESAS_Team::None;


	//functions
	UFUNCTION(BlueprintCallable)
	void SetTeam(ESAS_Team NewTeam);

	void RemoveUnitFromGame();


		
protected:

	UPROPERTY(BlueprintReadOnly)
	USAS_UnitManagerComponent* AssignedUnitManager;

};
