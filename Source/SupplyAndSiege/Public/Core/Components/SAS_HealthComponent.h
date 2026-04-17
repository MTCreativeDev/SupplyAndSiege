// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_HealthComponent.generated.h"

class USAS_DamageTypeData;
class USAS_HealthDefinitionData;
class USAS_HealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnHealthChanged,
	USAS_HealthComponent*, Sender,
	float, OldHealth,
	float, NewHealth
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,
	USAS_HealthComponent*, Sender
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAS_HealthComponent();

	UFUNCTION(BlueprintCallable, Category = "Health")
	float ApplyDamage(float RawDamage, USAS_DamageTypeData* DamageType);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeath OnDeath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 0.0f;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	USAS_HealthDefinitionData* HealthDefinitionData = nullptr;

	UPROPERTY(Transient)
	bool bIsInitialized = false;

	UPROPERTY(Transient)
	bool bIsDead = false;
};
