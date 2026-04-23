

#pragma once

#include "CoreMinimal.h"
#include "SAS_InventoryReservationRecord.generated.h"

class UItemDefinitionPrimaryData;

UENUM(BlueprintType)
enum class ESAS_InventoryReservationType : uint8
{
	None		UMETA(DisplayName = "None"),
	Outbound	UMETA(DisplayName = "Outbound"),
	Inbound		UMETA(DisplayName = "Inbound")
};


USTRUCT(BlueprintType)
struct SUPPLYANDSIEGE_API FSAS_InventoryReservationRecord
{
	GENERATED_BODY()

public:
	bool IsValid() const
	{
		return ReservationID.IsValid() && Item != nullptr && Quantity > 0 && ReservationType != ESAS_InventoryReservationType::None;
	}

	void Reset()
	{
		ReservationID.Invalidate();
		ReservationType = ESAS_InventoryReservationType::None;
		Item = nullptr;
		Quantity = 0;
		Claimer.Reset();
	}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Reservations")
	FGuid ReservationID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Reservations")
	ESAS_InventoryReservationType ReservationType = ESAS_InventoryReservationType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Reservations")
	TObjectPtr<UItemDefinitionPrimaryData> Item = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory Reservations")
	int32 Quantity = 0;

	UPROPERTY()
	TWeakObjectPtr<UObject> Claimer = nullptr;


};
