

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "Core/Interfaces/SAS_BuildingWithInventory.h"
#include "SAS_SB_ResourceDepot.generated.h"


class USAS_InventoryComponent;
/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_SB_ResourceDepot : public ASAS_SelectableBuilding, public ISAS_BuildingWithInventory
{
	GENERATED_BODY()

public:
	ASAS_SB_ResourceDepot();

	UFUNCTION(BlueprintCallable, Category = "Resource_Depot")
	const TArray<FVector> GetDropOffLocations() const;

	virtual USAS_InventoryComponent* GetInputInventory() override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Resource_Depot")
	void RebuildDropoffCache();


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	USAS_InventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USceneComponent* DropOffLocationContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource_Depot")
	bool bShowDropOffLocations = true;

	TArray<FVector> DropOffLocations_World;
	
};
