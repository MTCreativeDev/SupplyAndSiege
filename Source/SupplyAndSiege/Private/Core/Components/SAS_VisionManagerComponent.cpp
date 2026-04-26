// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/SAS_VisionManagerComponent.h"
#include "Core/Components/SAS_VisionComponent.h"
#include "Core/Components/SAS_UnitManagerComponent.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"

USAS_VisionManagerComponent::USAS_VisionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_VisionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::ResolveViewingTeam));
	}
}

void USAS_VisionManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(UpdateTimer);
		TM.ClearTimer(ImmediateRecomputeTimer);
		TM.ClearTimer(ResolveViewingTeamTimer);
	}

	Sources.Reset();
	AllTargets.Reset();
	LastVisibleToViewer.Reset();

	Super::EndPlay(EndPlayReason);
}

void USAS_VisionManagerComponent::RegisterComponent(USAS_VisionComponent* Comp)
{
	if (!IsValid(Comp))
	{
		return;
	}

	TWeakObjectPtr<USAS_VisionComponent> WeakComp(Comp);
	AllTargets.AddUnique(WeakComp);

	if (Comp->IsSource())
	{
		TArray<TWeakObjectPtr<USAS_VisionComponent>>& TeamSources = Sources.FindOrAdd(Comp->GetCachedTeam());
		TeamSources.AddUnique(WeakComp);
	}
}

void USAS_VisionManagerComponent::UnregisterComponent(USAS_VisionComponent* Comp, ESAS_Team Team)
{
	if (Comp == nullptr)
	{
		return;
	}

	TWeakObjectPtr<USAS_VisionComponent> WeakComp(Comp);
	AllTargets.RemoveSingle(WeakComp);

	if (TArray<TWeakObjectPtr<USAS_VisionComponent>>* TeamSources = Sources.Find(Team))
	{
		TeamSources->RemoveSingle(WeakComp);
	}

	LastVisibleToViewer.Remove(WeakComp);
}

void USAS_VisionManagerComponent::SetViewingTeam(ESAS_Team NewTeam)
{
	if (ViewingTeam == NewTeam && bViewingTeamResolved)
	{
		return;
	}
	ViewingTeam = NewTeam;
	bViewingTeamResolved = true;
	LastVisibleToViewer.Reset();
	RequestImmediateRecompute();
}

void USAS_VisionManagerComponent::RequestImmediateRecompute()
{
}

bool USAS_VisionManagerComponent::IsActorVisibleToViewer(AActor* Actor) const
{
	return true;
}

void USAS_VisionManagerComponent::RecomputeVisibility()
{
}

void USAS_VisionManagerComponent::ResolveViewingTeam()
{
	if (bViewingTeamResolved)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ASAS_PlayerController* PC = Cast<ASAS_PlayerController>(World->GetFirstPlayerController());
	if (!PC)
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::ResolveViewingTeam));
		return;
	}

	USAS_UnitManagerComponent* UnitManager = PC->FindComponentByClass<USAS_UnitManagerComponent>();
	if (!UnitManager)
	{
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::ResolveViewingTeam));
		return;
	}

	ViewingTeam = UnitManager->AssignedTeam;
	bViewingTeamResolved = true;
	RequestImmediateRecompute();
}

void USAS_VisionManagerComponent::PruneStaleEntries()
{
}
