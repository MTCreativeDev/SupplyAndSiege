#include "Core/Components/SAS_HealthComponent.h"

#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Misc/DataAssets/SAS_HealthDefinitionData.h"
#include "Misc/DataAssets/SAS_UnitTypeData.h"

USAS_HealthComponent::USAS_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = 0.0f;
	MaxHealth = 0.0f;
	HealthDefinitionData = nullptr;
	bIsInitialized = false;
	bIsDead = false;
}

void USAS_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent on '%s' could not find an owner."), *GetNameSafe(this));
		return;
	}

	USAS_UnitInformationComponent* UnitInformationComponent = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!UnitInformationComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent on '%s' could not find UnitInformationComponent."), *GetNameSafe(Owner));
		return;
	}

	USAS_UnitTypeData* UnitType = UnitInformationComponent->UnitType;
	HealthDefinitionData = UnitType ? UnitType->HealthDefinition : nullptr;
	if (!HealthDefinitionData)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent on '%s' is missing a HealthDefinition."), *GetNameSafe(Owner));
		return;
	}

	MaxHealth = HealthDefinitionData->MaxHealth;
	if (MaxHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent on '%s' has invalid MaxHealth: %f."), *GetNameSafe(Owner), MaxHealth);
		MaxHealth = 0.0f;
		return;
	}

	CurrentHealth = MaxHealth;
	bIsDead = false;
	bIsInitialized = true;
}

float USAS_HealthComponent::ApplyDamage(float RawDamage, USAS_DamageTypeData* DamageType)
{
	if (!bIsInitialized || bIsDead || RawDamage <= 0.0f)
	{
		return 0.0f;
	}

	const float PreviousHealth = CurrentHealth;

	float Resistance = 0.0f;
	if (HealthDefinitionData && DamageType)
	{
		if (const float* FoundResistance = HealthDefinitionData->DamageTypeResistances.Find(DamageType))
		{
			Resistance = *FoundResistance;
		}
	}

	Resistance = FMath::Clamp(Resistance, 0.0f, 1.0f);

	const float AdjustedDamage = RawDamage * (1.0f - Resistance);
	CurrentHealth = FMath::Clamp(PreviousHealth - AdjustedDamage, 0.0f, MaxHealth);

	const float AppliedDamage = PreviousHealth - CurrentHealth;
	if (AppliedDamage <= 0.0f)
	{
		return 0.0f;
	}

	OnHealthChanged.Broadcast(this, PreviousHealth, CurrentHealth);

	if (!bIsDead && CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnDeath.Broadcast(this);
	}

	return AppliedDamage;
}

float USAS_HealthComponent::GetHealthPercent() const
{
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}

	return CurrentHealth / MaxHealth;
}

bool USAS_HealthComponent::IsAlive() const
{
	return CurrentHealth > 0.0f;
}
