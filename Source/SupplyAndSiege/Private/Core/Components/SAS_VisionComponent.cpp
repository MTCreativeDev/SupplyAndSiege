// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/SAS_VisionComponent.h"

USAS_VisionComponent::USAS_VisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_VisionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USAS_VisionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USAS_VisionComponent::OnTeamChanged(ESAS_Team NewTeam)
{
}

void USAS_VisionComponent::TryRegisterWithManager()
{
}

void USAS_VisionComponent::ApplyDefaultHiddenOnRegistration()
{
}

void USAS_VisionComponent::SetHidden(bool bNewHidden)
{
}
