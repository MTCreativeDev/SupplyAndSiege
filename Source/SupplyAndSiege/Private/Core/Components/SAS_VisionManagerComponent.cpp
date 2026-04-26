// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/SAS_VisionManagerComponent.h"

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
}

void USAS_VisionManagerComponent::UnregisterComponent(USAS_VisionComponent* Comp, ESAS_Team Team)
{
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
