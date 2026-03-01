// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_ResourceManagerComponent.generated.h"


class USAS_ResourceTypeData;
class USAS_ResourceClusterComponent;
class UPrimitiveComponent;

//I normally like to create a separate class for structs, but I dont think this will be used elsewhere. If it is, I will move it to its own file.
USTRUCT()
struct FSAS_ResourceRuntimeState
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Remaining = 0;

	UPROPERTY()
	bool bDepleted = false;

	UPROPERTY()
	TObjectPtr<USAS_ResourceTypeData> TypeData = nullptr;
};

USTRUCT()
struct FSAS_ResourceReservationState
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> Claimer;

	UPROPERTY()
	double ExpiresAtSeconds = 0.0f;
};

/*===============================================
				End of Structs
================================================*/


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_ResourceManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USAS_ResourceManagerComponent();

	UFUNCTION(BlueprintCallable, Category = "Resource")
	int32 GetRemaining(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData) const;
	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool IsDepleted(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData) const;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool TryReserve(const FSAS_ResourceKey& Key, AActor* Claimer, float DurationSeconds = 10.f);
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void ReleaseReservation(const FSAS_ResourceKey& Key, AActor* Claimer);

	//TODO: Eventually this should be updated to allow a certain number of units to get the resource not just one.
	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool IsReservedByOther(const FSAS_ResourceKey& Key, AActor* Claimer) const;

	ESAS_ResourceValidity CheckValidity(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData, AActor* Claimer) const;


	UFUNCTION(BlueprintCallable, Category = "Resource")
	int32 ApplyHarvest(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData, int32 RequestedAmount, USAS_ResourceClusterComponent* ClusterForVisuals = nullptr, UPrimitiveComponent* HitComponentForVisuals = nullptr);



protected:

	virtual void BeginPlay() override;

private:

	double Now() const;
	void CleanupExpiredReservations();

	FSAS_ResourceRuntimeState& FindOrAddState_OnModify(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData);

private:

	UPROPERTY()
	TMap<FSAS_ResourceKey, FSAS_ResourceRuntimeState> ModifiedStates;

	UPROPERTY()
	TMap<FSAS_ResourceKey, FSAS_ResourceReservationState> Reservations;




};
