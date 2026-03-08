

#pragma once

#include "CoreMinimal.h"
#include "Core/Pawns/SAS_SelectablePawn.h"
#include "SAS_SP_InventoryCapable.generated.h"

class USAS_InventoryComponent;
class UBoxComponent;


UCLASS()
class SUPPLYANDSIEGE_API ASAS_SP_InventoryCapable : public ASAS_SelectablePawn
{
	GENERATED_BODY()
	
public:
	ASAS_SP_InventoryCapable();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void BeginLookingForInventory(USAS_InventoryComponent* InTargetInventory);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void StopLookingForInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	USAS_InventoryComponent* GetInventory() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	USAS_InventoryComponent* Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	UBoxComponent* InventoryCheckCollision;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bLookingForInventory = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<USAS_InventoryComponent> TargetInventory = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TWeakObjectPtr<AActor> TargetInventoryOwner;

};
