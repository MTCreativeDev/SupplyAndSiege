// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SAS_AIStatics.generated.h"

class USAS_UnitManagerComponent;

/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API USAS_AIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** This is the array of all available units to this player. Units include buildings, pawns etc. Anything that the player can select. */
	UFUNCTION(BlueprintCallable, meta=(KeyWords="add all"))
	static void AssignSelectableUnit(USAS_UnitManagerComponent* UnitManagerComp, AActor* NewUnit);
	
	/** This is the array of all available units to this player. Units include buildings, pawns etc. Anything that the player can select. */
	UFUNCTION(BlueprintCallable, meta=(KeyWords="all"))
	static TArray<AActor*> GetSelectableUnits(USAS_UnitManagerComponent* UnitManagerComp);
	
	/** Moves the selected units */
	UFUNCTION(BlueprintCallable, meta=(keywords="move to"))
	static void RightClickReceived(USAS_UnitManagerComponent* UnitManagerComp, FVector WorldLocation);
	
	/** Only the units currently selected. */
	UFUNCTION(BlueprintCallable)
	static void AddSelectedUnit(USAS_UnitManagerComponent* UnitManagerComp, AActor* Unit);
};
