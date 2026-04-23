

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_LmQueueWidget.generated.h"

class ASAS_PlayerController;
class USAS_UnitInformationComponent;
class USAS_WorkerControlComponent;

UCLASS()
class SUPPLYANDSIEGE_API USAS_LmQueueWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "LM Queue")
	void SetSelectedUnits(const TArray<USAS_UnitInformationComponent*>& InSelectedUnits);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "LM Queue")
	void RefreshWidget();

	UFUNCTION(BlueprintCallable, Category = "LM Queue")
	void HandleAddToLMQueueClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "LM Queue")
	void BP_RefreshWidget(bool bHasValidWorkers, int32 InValidWorkerCount, int32 InAlreadyQueuedCount);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "LM Queue")
	TArray<TObjectPtr<USAS_UnitInformationComponent>> SelectedUnits;

	UPROPERTY(BlueprintReadOnly, Category = "LM Queue")
	TArray<TObjectPtr<USAS_WorkerControlComponent>> ValidWorkerComponents;

	UPROPERTY(BlueprintReadOnly, Category = "LM Queue")
	int32 AlreadyQueuedCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "LM Queue")
	TObjectPtr<ASAS_PlayerController> PlayerController = nullptr;
	
};
