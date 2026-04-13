

#pragma once

#include "CoreMinimal.h"
#include "SAS_InventoryReservationHandle.generated.h"

USTRUCT(BlueprintType)
struct SUPPLYANDSIEGE_API FSAS_InventoryReservationHandle
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logistics")
	FGuid ReservationID;

	bool IsValid() const
	{
		return ReservationID.IsValid();
	}

	void Reset()
	{
		ReservationID.Invalidate();
	}
};
