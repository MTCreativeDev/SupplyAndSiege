// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_InventorySlot.h"
#include "Core/SAS_Enumerators.h"
#include "Engine/AssetManagerTypes.h"
#include "Misc/Structs/SAS_InventoryReservationRecord.h"
#include "SAS_InventoryComponent.generated.h"


class USAS_InventoryProfileData;
class USAS_UnitInformationComponent;
class UItemDefinitionPrimaryData;
struct FSAS_InventoryReservationHandle;

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
	virtual int32 AddItem(UItemDefinitionPrimaryData* Item, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual int32 RemoveItem(UItemDefinitionPrimaryData* Item, int32 Quantity);

	void SetInventoryProfile(USAS_InventoryProfileData* NewProfile);

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	int32 GetTotalItemAmount(UItemDefinitionPrimaryData* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	int32 GetReservedOutboundAmount(UItemDefinitionPrimaryData* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	int32 GetReservedInboundAmount(UItemDefinitionPrimaryData* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	int32 GetUnreservedItemAmount(UItemDefinitionPrimaryData* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	int32 GetAvailableInboundCapacity (UItemDefinitionPrimaryData* Item, int32 RequestedQuantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	bool ReserveOutbound(UItemDefinitionPrimaryData* Item, int32 RequestedQuantity, UObject* Claimer, FSAS_InventoryReservationHandle& OutHandle, int32& OutReservedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	bool ReserveInbound(UItemDefinitionPrimaryData* Item, int32 RequestedQuantity, UObject* Claimer, FSAS_InventoryReservationHandle& OutHandle, int32& OutReservedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	bool ReleaseReservation(FSAS_InventoryReservationHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	bool FinalizeOutboundPickup(FSAS_InventoryReservationHandle Handle, int32& OutRemovedQuantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory Reservations")
	bool FinalizeInboundDropoff(FSAS_InventoryReservationHandle Handle, int32 QuantityToDeliver, int32& OutAddedQuantity);

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

	int32 AddItem_Internal(UItemDefinitionPrimaryData* Item, int32 Quantity);

	int32 RemoveItem_Internal(UItemDefinitionPrimaryData* Item, int32 Quantity);

	int32 GetMaxStack(const UItemDefinitionPrimaryData* Item) const;

	int32 GetPhysicalOverflow(UItemDefinitionPrimaryData* Item, int32 Quantity) const;

	bool CreateReservationRecord(ESAS_InventoryReservationType ReservationType, UItemDefinitionPrimaryData* Item, int32 Quantity, UObject* Claimer, FSAS_InventoryReservationHandle& OutHandle);

	bool GetReservationRecord(FSAS_InventoryReservationHandle Handle, FSAS_InventoryReservationRecord*& OutRecord);

	int32 GetReservationAwareOverflow(UItemDefinitionPrimaryData* Item, int32 Quantity) const;


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

	UPROPERTY()
	TMap<FGuid, FSAS_InventoryReservationRecord> ActiveReservations;
		
};
