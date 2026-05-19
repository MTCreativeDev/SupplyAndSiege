


#include "Core/Components/SAS_CombatComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_HealthComponent.h"
#include "Misc/DataAssets/SAS_AttackDefinitionData.h"


USAS_CombatComponent::USAS_CombatComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

bool USAS_CombatComponent::SetCombatTarget(AActor* NewTarget)
{
	if (!IsValidEnemyTarget(NewTarget)) return false;

	CurrentTarget = NewTarget;
	OnCombatTargetChanged.Broadcast(NewTarget);

	return true;
}

void USAS_CombatComponent::ClearCombatTarget()
{
	AActor* OldTarget = CurrentTarget.Get();

	CurrentTarget.Reset();

	if (OldTarget)
	{
		OnCombatTargetLost.Broadcast(OldTarget);
	}
}

bool USAS_CombatComponent::HasValidTarget() const
{
	return IsValidEnemyTarget(CurrentTarget.Get());
}

bool USAS_CombatComponent::IsTargetInAttackRange() const
{
	if (!AttackData) return false;
	if (!HasValidTarget()) return false;

	return GetDistanceToTarget() <= AttackData->AttackRange;
}

bool USAS_CombatComponent::CanAttackNow() const
{
	if (!AttackData) return false;
	if (!HasValidTarget()) return false;
	if (!IsTargetInAttackRange()) return false;

	const UWorld* World = GetWorld();
	if (!World) return false;

	const float CurrentTime = World->GetTimeSeconds();
	return CurrentTime >= LastAttackTime + AttackData->Cooldown;
}

bool USAS_CombatComponent::TryExecuteAttack()
{
	if (!CanAttackNow()) return false;

	USAS_HealthComponent* TargetHealth = GetTargetHealthComponent();
	if (!TargetHealth) return false;

	TargetHealth->ApplyDamage(AttackData->Damage, AttackData->DamageType);

	LastAttackTime = GetWorld()->GetTimeSeconds();

	OnAttackExecuted.Broadcast(CurrentTarget.Get(), AttackData);

	return true;
}


void USAS_CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();

	if (OwnerActor)
	{
		OwnerUnitInfo = OwnerActor->FindComponentByClass<USAS_UnitInformationComponent>();
	}
}

bool USAS_CombatComponent::IsValidEnemyTarget(AActor* TargetActor) const
{
	if (!TargetActor || TargetActor == OwnerActor) return false;
	if (!OwnerUnitInfo) return false;

	const USAS_UnitInformationComponent* TargetUnitInfo = TargetActor->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!TargetUnitInfo) return false;

	return TargetUnitInfo->GetTeam() != OwnerUnitInfo->GetTeam();
}

float USAS_CombatComponent::GetDistanceToTarget() const
{
	if (!OwnerActor || !CurrentTarget.IsValid()) return TNumericLimits<float>::Max();

	return FVector::Dist(
		OwnerActor->GetActorLocation(),
		CurrentTarget->GetActorLocation()
	);
}

USAS_HealthComponent* USAS_CombatComponent::GetTargetHealthComponent() const
{
	AActor* TargetActor = CurrentTarget.Get();
	if (!TargetActor) return nullptr;

	return TargetActor->FindComponentByClass<USAS_HealthComponent>();
}


