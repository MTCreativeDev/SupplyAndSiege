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

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ResolveViewingTeamTimer = World->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::ResolveViewingTeam));

	World->GetTimerManager().SetTimer(
		UpdateTimer, this, &USAS_VisionManagerComponent::RecomputeVisibility,
		UpdateIntervalSeconds, /*bLoop=*/true);
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

	RequestImmediateRecompute();
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
	RequestImmediateRecompute();
}

void USAS_VisionManagerComponent::RequestImmediateRecompute()
{
	if (bImmediateRecomputeQueued)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bImmediateRecomputeQueued = true;
	ImmediateRecomputeTimer = World->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::RecomputeVisibility));
}

bool USAS_VisionManagerComponent::IsActorVisibleToViewer(AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return false;
	}

	for (const TWeakObjectPtr<USAS_VisionComponent>& Weak : LastVisibleToViewer)
	{
		USAS_VisionComponent* Comp = Weak.Get();
		if (Comp && Comp->GetOwner() == Actor)
		{
			return true;
		}
	}
	return false;
}

void USAS_VisionManagerComponent::RecomputeVisibility()
{
	bImmediateRecomputeQueued = false;

	if (!bViewingTeamResolved || ViewingTeam == ESAS_Team::None)
	{
		return;
	}

	TSet<TWeakObjectPtr<USAS_VisionComponent>> NewVisible;
	NewVisible.Reserve(AllTargets.Num());

	for (const TWeakObjectPtr<USAS_VisionComponent>& WeakTarget : AllTargets)
	{
		USAS_VisionComponent* Target = WeakTarget.Get();
		if (!Target)
		{
			continue;
		}
		if (Target->GetCachedTeam() == ViewingTeam)
		{
			NewVisible.Add(WeakTarget);
		}
	}

	if (TArray<TWeakObjectPtr<USAS_VisionComponent>>* TeamSources = Sources.Find(ViewingTeam))
	{
		for (const TWeakObjectPtr<USAS_VisionComponent>& WeakSource : *TeamSources)
		{
			USAS_VisionComponent* Source = WeakSource.Get();
			if (!Source)
			{
				continue;
			}
			AActor* SourceOwner = Source->GetOwner();
			if (!IsValid(SourceOwner))
			{
				continue;
			}

			const FVector SourceLoc = SourceOwner->GetActorLocation();
			const float RadiusSq = Source->GetRadiusSquared();

			for (const TWeakObjectPtr<USAS_VisionComponent>& WeakTarget : AllTargets)
			{
				if (NewVisible.Contains(WeakTarget))
				{
					continue;
				}
				USAS_VisionComponent* Target = WeakTarget.Get();
				if (!Target)
				{
					continue;
				}
				AActor* TargetOwner = Target->GetOwner();
				if (!IsValid(TargetOwner))
				{
					continue;
				}

				FVector Delta = TargetOwner->GetActorLocation() - SourceLoc;
				Delta.Z = 0.0f;
				if (Delta.SizeSquared() <= RadiusSq)
				{
					NewVisible.Add(WeakTarget);
				}
			}
		}
	}

	for (const TWeakObjectPtr<USAS_VisionComponent>& WeakTarget : NewVisible)
	{
		if (LastVisibleToViewer.Contains(WeakTarget))
		{
			continue;
		}
		if (USAS_VisionComponent* Target = WeakTarget.Get())
		{
			Target->SetHidden(false);
		}
	}

	for (const TWeakObjectPtr<USAS_VisionComponent>& WeakTarget : LastVisibleToViewer)
	{
		if (NewVisible.Contains(WeakTarget))
		{
			continue;
		}
		if (USAS_VisionComponent* Target = WeakTarget.Get())
		{
			Target->SetHidden(true);
		}
	}

	LastVisibleToViewer = MoveTemp(NewVisible);

	++TickCounter;
	if (PruneEveryNTicks > 0 && (TickCounter % PruneEveryNTicks) == 0)
	{
		PruneStaleEntries();
	}
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
		ResolveViewingTeamTimer = World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::ResolveViewingTeam));
		return;
	}

	USAS_UnitManagerComponent* UnitManager = PC->FindComponentByClass<USAS_UnitManagerComponent>();
	if (!UnitManager)
	{
		ResolveViewingTeamTimer = World->GetTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &USAS_VisionManagerComponent::ResolveViewingTeam));
		return;
	}

	ViewingTeam = UnitManager->AssignedTeam;
	bViewingTeamResolved = true;
	RequestImmediateRecompute();
}

void USAS_VisionManagerComponent::PruneStaleEntries()
{
	AllTargets.RemoveAll([](const TWeakObjectPtr<USAS_VisionComponent>& Weak) { return !Weak.IsValid(); });

	for (auto It = Sources.CreateIterator(); It; ++It)
	{
		It.Value().RemoveAll([](const TWeakObjectPtr<USAS_VisionComponent>& Weak) { return !Weak.IsValid(); });
		if (It.Value().Num() == 0)
		{
			It.RemoveCurrent();
		}
	}

	for (auto It = LastVisibleToViewer.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}
}
