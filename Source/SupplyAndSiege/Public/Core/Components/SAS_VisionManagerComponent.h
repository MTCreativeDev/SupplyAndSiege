// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_VisionManagerComponent.generated.h"

class USAS_VisionComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_VisionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAS_VisionManagerComponent();

	void RegisterComponent(USAS_VisionComponent* Comp);
	void UnregisterComponent(USAS_VisionComponent* Comp, ESAS_Team Team);

	void SetViewingTeam(ESAS_Team NewTeam);
	void RequestImmediateRecompute();

	ESAS_Team GetViewingTeam() const { return ViewingTeam; }
	bool IsViewingTeamResolved() const { return bViewingTeamResolved; }

	UFUNCTION(BlueprintCallable, Category = "Vision")
	bool IsActorVisibleToViewer(AActor* Actor) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void RecomputeVisibility();
	void ResolveViewingTeam();
	void PruneStaleEntries();

	TMap<ESAS_Team, TArray<TWeakObjectPtr<USAS_VisionComponent>>> Sources;
	TArray<TWeakObjectPtr<USAS_VisionComponent>> AllTargets;
	TSet<TWeakObjectPtr<USAS_VisionComponent>> LastVisibleToViewer;

	ESAS_Team ViewingTeam = ESAS_Team::None;
	bool bViewingTeamResolved = false;
	bool bImmediateRecomputeQueued = false;
	int32 TickCounter = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vision", meta = (AllowPrivateAccess = "true"))
	float UpdateIntervalSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Vision")
	int32 PruneEveryNTicks = 50;

	FTimerHandle UpdateTimer;
	FTimerHandle ImmediateRecomputeTimer;
	FTimerHandle ResolveViewingTeamTimer;
};
