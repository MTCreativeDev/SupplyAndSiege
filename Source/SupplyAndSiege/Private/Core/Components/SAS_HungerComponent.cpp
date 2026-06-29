


#include "Core/Components/SAS_HungerComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Misc/DataAssets/ItemDefinitionPrimaryData.h"

USAS_HungerComponent::USAS_HungerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 5.f;
}

void USAS_HungerComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHunger = FMath::Clamp(StartingHunger, 0.f, MaxHunger);
	PrimaryComponentTick.TickInterval = HungerTickInterval;

	CacheRequiredComponents();
	EvaluateHungerState();
}

void USAS_HungerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CacheRequiredComponents()) return;

	DrainHunger(DeltaTime);

	if (CurrentHunger <= HungryThreshold)
	{
		TryEat();
	}

	EvaluateHungerState();
}

bool USAS_HungerComponent::CacheRequiredComponents()
{
	
	if (!LogisticsManagerComponent)
	{
		UWorld* World = GetWorld();
		if (!World) return false;

		AGameStateBase* GameState = World->GetGameState();
		if (!IsValid(GameState)) return false;

		LogisticsManagerComponent = GameState->FindComponentByClass<USAS_LogisticsManagerComponent>();
	}

	return IsValid(LogisticsManagerComponent);
}

void USAS_HungerComponent::DrainHunger(float DeltaTime)
{
	const float OldHunger = CurrentHunger;

	CurrentHunger = FMath::Clamp(
		CurrentHunger - HungerDrainPerSecond * DeltaTime,
		0.f,
		MaxHunger
	);

	if (!FMath::IsNearlyEqual(OldHunger, CurrentHunger))
	{
		OnHungerValueChanged.Broadcast(CurrentHunger);
	}
}

bool USAS_HungerComponent::TryEat()
{
	if (!IsValid(LogisticsManagerComponent)) return false;
	if (FoodAmountToConsume <= 0) return false;


	const bool bConsumedFood = LogisticsManagerComponent->TryConsumeFood();

	if (!bConsumedFood)
	{
		return false;
	}

	RestoreHunger(FoodRestoreAmount);
	return true;
}

void USAS_HungerComponent::RestoreHunger(float Amount)
{
	if (Amount <= 0.f) return;

	const float OldHunger = CurrentHunger;

	CurrentHunger = FMath::Clamp(
		CurrentHunger + Amount,
		0.f,
		MaxHunger
	);

	if (!FMath::IsNearlyEqual(OldHunger, CurrentHunger))
	{
		OnHungerValueChanged.Broadcast(CurrentHunger);
	}

	EvaluateHungerState();
}

void USAS_HungerComponent::EvaluateHungerState()
{
	ESAS_HungerState NewState = ESAS_HungerState::Full;

	if (CurrentHunger <= 0)
	{
		NewState = ESAS_HungerState::StarvedToDeath;
		SetHungerState(NewState);

		if (AActor* Owner = GetOwner())
		{
			Owner->Destroy();
		}

		return;
	}

	if (CurrentHunger <= StarvingThreshold)
	{
		NewState = ESAS_HungerState::Starving;
		SetHungerState(NewState);
		return;		
	}

	if (CurrentHunger <= HungryThreshold)
	{
		NewState = ESAS_HungerState::Hungry;
		SetHungerState(NewState);
		return;
	}

	SetHungerState(NewState);
}

void USAS_HungerComponent::SetHungerState(ESAS_HungerState NewState)
{
	if (HungerState == NewState) return;

	const ESAS_HungerState OldState = HungerState;
	HungerState = NewState;

	OnHungerStateChanged.Broadcast(OldState, NewState);
}

float USAS_HungerComponent::GetHungerPercent() const
{
	if (MaxHunger <= 0.f) return 0.f;

	return CurrentHunger / MaxHunger;
}

bool USAS_HungerComponent::IsHungry() const
{
	return HungerState == ESAS_HungerState::Hungry
		|| HungerState == ESAS_HungerState::Starving;
}

bool USAS_HungerComponent::IsStarving() const
{
	return HungerState == ESAS_HungerState::Starving;
}
