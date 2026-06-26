

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_HungerComponent.generated.h"

class UItemDefinitionPrimaryData;
class USAS_UnitInformationComponent;
class USAS_LogisticsManagerComponent;

UENUM(BlueprintType)
enum class ESAS_HungerState : uint8
{
	Full		UMETA(DisplayName = "Full"),
	Hungry		UMETA(DisplayName = "Hungry"),
	Starving	UMETA(DisplayName = "Starving"),
	StarvedToDeath	UMETA(DisplayName = "Starved To Death")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHungerStateChanged,ESAS_HungerState, OldState,ESAS_HungerState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHungerValueChanged,float, NewHunger);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_HungerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_HungerComponent();

	UFUNCTION(BlueprintCallable, Category = "Hunger")
	void RestoreHunger(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Hunger")
	bool TryEat();

	UFUNCTION(BlueprintPure, Category = "Hunger")
	float GetCurrentHunger() const { return CurrentHunger; }

	UFUNCTION(BlueprintPure, Category = "Hunger")
	float GetMaxHunger() const { return MaxHunger; }

	UFUNCTION(BlueprintPure, Category = "Hunger")
	float GetHungerPercent() const;

	UFUNCTION(BlueprintPure, Category = "Hunger")
	ESAS_HungerState GetHungerState() const { return HungerState; }

	UFUNCTION(BlueprintPure, Category = "Hunger")
	bool IsHungry() const;

	UFUNCTION(BlueprintPure, Category = "Hunger")
	bool IsStarving() const;


	//Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Hunger")
	FOnHungerStateChanged OnHungerStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Hunger")
	FOnHungerValueChanged OnHungerValueChanged;


protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(	float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

	void DrainHunger(float DeltaTime);
	void EvaluateHungerState();
	void SetHungerState(ESAS_HungerState NewState);
	bool CacheRequiredComponents();


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float MaxHunger = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float StartingHunger = 300.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Hunger")
	float CurrentHunger = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HungerDrainPerSecond = .56f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HungryThreshold = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float StarvingThreshold = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float FoodRestoreAmount = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	int32 FoodAmountToConsume = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hunger")
	float HungerTickInterval = 5.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Hunger")
	ESAS_HungerState HungerState = ESAS_HungerState::Full;

	UPROPERTY()
	TObjectPtr<USAS_LogisticsManagerComponent> LogisticsManagerComponent;
		
};
