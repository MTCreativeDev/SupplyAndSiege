// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_VisionComponent.generated.h"

class USAS_VisionDefinitionData;
class USAS_UnitInformationComponent;
class USAS_VisionManagerComponent;
class UPrimitiveComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_VisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAS_VisionComponent();

	ESAS_Team GetCachedTeam() const { return CachedTeam; }
	bool IsSource() const { return bIsSource; }
	float GetRadiusSquared() const { return CachedRadiusSq; }
	float GetCachedRadius() const { return CachedRadius; }

	/** Called by the manager when the visible-set diff transitions this component. */
	void SetHidden(bool bNewHidden);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnTeamChanged(ESAS_Team NewTeam);

	void TryRegisterWithManager();
	void ApplyDefaultHiddenOnRegistration();

private:
	UPROPERTY()
	USAS_VisionDefinitionData* CachedDefinition = nullptr;

	float CachedRadius = 0.0f;
	float CachedRadiusSq = 0.0f;
	ESAS_Team CachedTeam = ESAS_Team::None;
	bool bIsSource = false;
	bool bRegistered = false;
	bool bIsHiddenLocally = false;
	bool bInitializedFromUnitType = false;
	bool bHasEndedPlay = false;

	int32 RetryCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Vision")
	int32 MaxRegisterRetries = 30;

	TWeakObjectPtr<USAS_UnitInformationComponent> CachedInfoComp;
	TWeakObjectPtr<USAS_VisionManagerComponent> CachedManager;
	TMap<TWeakObjectPtr<UPrimitiveComponent>, ECollisionResponse> CachedSelectableTraceResponses;
};
