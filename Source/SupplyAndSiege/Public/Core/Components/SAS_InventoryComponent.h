// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_InventorySlot.h"
#include "Core/SAS_Enumerators.h"
#include "Engine/AssetManagerTypes.h"
#include "SAS_InventoryComponent.generated.h"


class USAS_InventoryProfileData;
class USAS_UnitInformationComponent;
class UItemDefinitionPrimaryData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnInventoryChanged,
	USAS_InventoryComponent*, Sender,
	FPrimaryAssetId, ItemId,
	int32, Delta
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USAS_InventoryComponent();

	ESAS_Team GetAssignedTeam();

	USAS_InventoryProfileData* GetInventoryProfile();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GetAllItemTotals(TMap<FPrimaryAssetId, int32>& OutTotals) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddItem(UItemDefinitionPrimaryData* Item, int32 Quantity);

	//Dispatchers

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleTeamChanged(ESAS_Team NewTeam);


private:

	void GetUnitInformationAndBindToTeamChange();

	void CreateSlotsFromProfile();

public:	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	USAS_InventoryProfileData* InventoryProfile = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	USAS_UnitInformationComponent* UnitInfoComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FSAS_InventorySlot> Slots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESAS_Team AssignedTeam = ESAS_Team::None;
		
};
