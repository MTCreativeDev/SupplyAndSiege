// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/SAS_Enumerators.h"
#include "Core/Objects/SAS_SelectionInventoryViewModel.h"
#include "SAS_PlayerScreenWidget.generated.h"


class ASAS_PlayerController;
class USAS_UnitManagerComponent;
class USAS_UnitInformationComponent;
class USAS_SelectionInventoryViewModel;

UCLASS()
class SUPPLYANDSIEGE_API UPlayerScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleSelectedUnitsChanged(const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>& NewSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit Selection")
	void BP_SelectedUnitsChanged(const TArray<USAS_UnitInformationComponent*>& NewSelection);
	virtual void BP_SelectedUnitsChanged_Implementation(const TArray<USAS_UnitInformationComponent*>& NewSelection);

	UFUNCTION()
	void HandleSelectionInventoryModelChanged();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Selection Inventory")
	void BP_OnSelectionInventoryChanged(ESAS_SelectionInventoryMode Mode, const TMap<FPrimaryAssetId, int32>& Totals);
	virtual void BP_OnSelectionInventoryChanged_Implementation(ESAS_SelectionInventoryMode Mode, const TMap<FPrimaryAssetId, int32>& Totals);

	UFUNCTION(BlueprintCallable, Category = "Player_UI")
	void NotifyControllerCurrentlyInSubMenu();
	UFUNCTION(BlueprintCallable, Category = "Player_UI")
	void NotifyControllerNotInSubMenu();

private:




protected:
	
	UPROPERTY(BlueprintReadOnly)
	ASAS_PlayerController* PlayerController = nullptr;

	
	UPROPERTY(BlueprintReadOnly)
	USAS_UnitManagerComponent* UnitManagerComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USAS_SelectionInventoryViewModel> InventoryViewModel = nullptr;


};