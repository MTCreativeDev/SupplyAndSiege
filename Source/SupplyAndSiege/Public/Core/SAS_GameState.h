// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_GameState.generated.h"

class USAS_IslandManagerComponent;
class USAS_InventoryManagerComponent;
class USAS_ResourceManagerComponent;
class USAS_LogisticsManagerComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	USAS_ResourceManagerComponent* ResourceManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Island")
	USAS_IslandManagerComponent* IslandManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics")
	USAS_LogisticsManagerComponent* LogisticsManagerComponent;


};
