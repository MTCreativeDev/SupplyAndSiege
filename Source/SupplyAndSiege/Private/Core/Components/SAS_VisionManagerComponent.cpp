// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/SAS_VisionManagerComponent.h"
#include "Core/Components/SAS_VisionComponent.h"

USAS_VisionManagerComponent::USAS_VisionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_VisionManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USAS_VisionManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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
}

void USAS_VisionManagerComponent::PruneStaleEntries()
{
}
