

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_CombatComponent.generated.h"

class USAS_AttackDefinitionData;
class USAS_UnitInformationComponent;
class USAS_HealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatTargetChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatTargetLost, AActor*, LostTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackExecuted, AActor*, TargetActor, USAS_AttackDefinitionData*, AttackData);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_CombatComponent();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool SetCombatTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearCombatTarget();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool HasValidTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInAttackRange() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CanAttackNow() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool TryExecuteAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	AActor* GetCombatTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	USAS_AttackDefinitionData* GetAttackData() const { return AttackData; }

	/************************************
	Dispatchers
	************************************/

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnCombatTargetChanged OnCombatTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnCombatTargetLost OnCombatTargetLost;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnAttackExecuted OnAttackExecuted;
	

protected:

	virtual void BeginPlay() override;

	bool IsValidEnemyTarget(AActor* TargetActor) const;
	float GetDistanceToTarget() const;
	USAS_HealthComponent* GetTargetHealthComponent() const;

public:	

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USAS_AttackDefinitionData> AttackData;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	TObjectPtr<USAS_UnitInformationComponent> OwnerUnitInfo;

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

	float LastAttackTime = -9999.f;

		
};
