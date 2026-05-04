// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "SAS_UnitManagerComponent.generated.h"

class UEnvQuery;
class USAS_UnitControlComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitSelectionChange, const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>&, SelectedUnits);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_UnitManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAS_UnitManagerComponent();
	void SetTeam(ESAS_Team NewTeam);
	void AssignSelectableUnit(TWeakObjectPtr<AActor> NewUnit, bool BypassComponentCheck);
	void RemoveSelectableUnit(TWeakObjectPtr<AActor> UnitToRemove);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SetSelectedUnits(const TArray<USAS_UnitInformationComponent*>& NewSelection);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void ClearAllSelectedUnits();

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void AddSelectedUnit(USAS_UnitInformationComponent* UnitInfo);

	void RightClickReceived(const FHitResult Hit);
	void IssueMoveOrderToUnits(const TArray<TWeakObjectPtr<USAS_UnitControlComponent>>& UnitsToMove, const FVector& WorldLocation);
	const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>& GetSelectedUnits() const { return SelectedUnits; }

	//Dispatchers
	UPROPERTY(BlueprintAssignable)
	FOnUnitSelectionChange OnUnitSelectionChange;



protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnFormationQueryComplete(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);



private:
	
	USAS_UnitControlComponent* GetUnitControlFromInfo(USAS_UnitInformationComponent* UnitInfo) const;

	void HandleGroundRightClickForSelectedUnits(const FVector& WorldLocation);
	void HandleResourceRightClickForSelectedUnits(const FHitResult& Hit);

public:
	//This is the array of all available units to this player. Units include buildings, pawns etc. Anything that the player can select.
	TArray<TWeakObjectPtr<AActor>> SelectableUnits;

	UPROPERTY()
	TArray<TWeakObjectPtr<USAS_UnitInformationComponent>> SelectedUnits;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESAS_Team AssignedTeam = ESAS_Team::None;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FVector LastRightClickLocation;

protected:
	/** The movement formation location query to run. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UEnvQuery> FormationQuery;
	


private:

	TArray<TWeakObjectPtr<USAS_UnitControlComponent>> PendingFormationUnits;


};
