// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_PlayerScreenWidget.generated.h"

class ASAS_PlayerController;
class USAS_UnitManagerComponent;
class USAS_UnitInformationComponent;

UCLASS()
class SUPPLYANDSIEGE_API UPlayerScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:


protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleSelectedUnitsChanged(TArray<TWeakObjectPtr<USAS_UnitInformationComponent>> NewSelection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Unit Selection")
	void BP_SelectedUnitsChanged(const TArray<USAS_UnitInformationComponent*>& NewSelection);
	virtual void BP_SelectedUnitsChanged_Implementation(const TArray<USAS_UnitInformationComponent*>& NewSelection);

private:




protected:
	
	UPROPERTY(BlueprintReadOnly)
	ASAS_PlayerController* PlayerController = nullptr;

	
	UPROPERTY(BlueprintReadOnly)
	USAS_UnitManagerComponent* UnitManagerComponent = nullptr;

};