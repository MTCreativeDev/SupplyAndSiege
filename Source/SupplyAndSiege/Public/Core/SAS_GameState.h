// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_GameState.generated.h"

class USAS_InventoryManagerComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ASAS_GameState();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	USAS_InventoryManagerComponent* GetInventoryManagerForTeam(ESAS_Team Team) const;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	USAS_InventoryManagerComponent* InventoryManagerComponentTeam1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	USAS_InventoryManagerComponent* InventoryManagerComponentTeam2;

};
