// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/SAS_VisionComponent.h"

#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_VisionManagerComponent.h"
#include "Core/SAS_GameState.h"
#include "Misc/DataAssets/SAS_UnitTypeData.h"
#include "Misc/DataAssets/SAS_VisionDefinitionData.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

USAS_VisionComponent::USAS_VisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_VisionComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogTemp, Error, TEXT("USAS_VisionComponent: Owner is invalid in BeginPlay."));
		return;
	}

	USAS_UnitInformationComponent* InfoComp = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!InfoComp)
	{
		UE_LOG(LogTemp, Error, TEXT("USAS_VisionComponent: %s has no USAS_UnitInformationComponent. Vision component will not register."), *Owner->GetName());
		return;
	}

	CachedInfoComp = InfoComp;

	if (InfoComp->UnitType == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("USAS_VisionComponent: %s has UnitInformationComponent with null UnitType. Registering as target-only."), *Owner->GetName());
	}
	else
	{
		CachedDefinition = InfoComp->UnitType->VisionDefinition;
		if (CachedDefinition)
		{
			if (CachedDefinition->Radius > 0.0f)
			{
				CachedRadius = CachedDefinition->Radius;
				CachedRadiusSq = CachedRadius * CachedRadius;
				bIsSource = true;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("USAS_VisionComponent: %s has VisionDefinition with non-positive Radius (%f). Registering as target-only."), *Owner->GetName(), CachedDefinition->Radius);
				CachedDefinition = nullptr;
			}
		}
	}

	CachedTeam = InfoComp->AssignedTeam;
	bInitializedFromUnitType = true;

	InfoComp->NotifyTeamChange.AddDynamic(this, &USAS_VisionComponent::OnTeamChanged);

	TryRegisterWithManager();
}

void USAS_VisionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bHasEndedPlay = true;

	if (USAS_UnitInformationComponent* InfoComp = CachedInfoComp.Get())
	{
		InfoComp->NotifyTeamChange.RemoveDynamic(this, &USAS_VisionComponent::OnTeamChanged);
	}

	if (bRegistered)
	{
		if (USAS_VisionManagerComponent* Manager = CachedManager.Get())
		{
			Manager->UnregisterComponent(this, CachedTeam);
		}
		bRegistered = false;
	}

	Super::EndPlay(EndPlayReason);
}

void USAS_VisionComponent::OnTeamChanged(ESAS_Team NewTeam)
{
}

void USAS_VisionComponent::TryRegisterWithManager()
{
	if (bHasEndedPlay || bRegistered || !bInitializedFromUnitType)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ASAS_GameState* GS = World->GetGameState<ASAS_GameState>();
	USAS_VisionManagerComponent* Manager = GS ? GS->GetVisionManager() : nullptr;

	if (!Manager)
	{
		++RetryCount;
		if (RetryCount > MaxRegisterRetries)
		{
			UE_LOG(LogTemp, Error, TEXT("USAS_VisionComponent on %s exhausted manager-resolution retries (%d). Component will not register."),
				GetOwner() ? *GetOwner()->GetName() : TEXT("<no owner>"), MaxRegisterRetries);
			return;
		}
		World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &USAS_VisionComponent::TryRegisterWithManager));
		return;
	}

	CachedManager = Manager;
	Manager->RegisterComponent(this);
	bRegistered = true;
}

void USAS_VisionComponent::ApplyDefaultHiddenOnRegistration()
{
}

void USAS_VisionComponent::SetHidden(bool bNewHidden)
{
}
