

#include "Core/Components/SAS_ResourceManagerComponent.h"
#include "Misc/DataAssets/SAS_ResourceTypeData.h"
#include "Core/Components/SAS_ResourceClusterComponent.h"
#include "Engine/World.h"


USAS_ResourceManagerComponent::USAS_ResourceManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

int32 USAS_ResourceManagerComponent::GetRemaining(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData) const
{
	if (!Key.IsValid() || !TypeData) return 0;

	if (const FSAS_ResourceRuntimeState* State = ModifiedStates.Find(Key))
	{
		return State->Remaining;
	}

	return TypeData->StartingAmount;
}

bool USAS_ResourceManagerComponent::IsDepleted(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData) const
{
	return GetRemaining(Key, TypeData) <= 0;
}

bool USAS_ResourceManagerComponent::TryReserve(const FSAS_ResourceKey& Key, AActor* Claimer, float DurationSeconds)
{
	if (!Key.IsValid() || !Claimer) return false;

	CleanupExpiredReservations();

	const double Exp = (DurationSeconds > 0.f) ? (Now() + DurationSeconds) : 0.0;

	FSAS_ResourceReservationState* Existing = Reservations.Find(Key);
	if (!Existing)
	{
		FSAS_ResourceReservationState NewRes;
		NewRes.Claimer = Claimer;
		NewRes.ExpiresAtSeconds = Exp;
		Reservations.Add(Key, NewRes);
		return true;
	}

	if (Existing->Claimer.IsValid() && Existing->Claimer.Get() == Claimer)
	{
		//In case the claimer re-claims, update to the new time
		Existing->ExpiresAtSeconds = Exp;
		return true;
	}

	const double T = Now();
	if (Existing->Claimer.IsValid() && (Existing->ExpiresAtSeconds == 00 || Existing->ExpiresAtSeconds > T)) return false;

	//If the existing claimer is no longer valid or the time has expired.
	Existing->Claimer = Claimer;
	Existing->ExpiresAtSeconds = Exp;
	return true;

}

void USAS_ResourceManagerComponent::ReleaseReservation(const FSAS_ResourceKey& Key, AActor* Claimer)
{
	if (!Key.IsValid()) return;

	FSAS_ResourceReservationState* Existing = Reservations.Find(Key);
	if (!Existing) return;

	if (Existing->Claimer.IsValid() && Existing->Claimer.Get() == Claimer)
	{
		Reservations.Remove(Key);
	}
}

bool USAS_ResourceManagerComponent::IsReservedByOther(const FSAS_ResourceKey& Key, AActor* Claimer) const
{
	if (!Key.IsValid() || !Claimer) return false;

	const FSAS_ResourceReservationState* Res = Reservations.Find(Key);
	if (!Res) return false;

	const double T = Now();
	if (Res->ExpiresAtSeconds > 0.0 && Res->ExpiresAtSeconds <= T)
	{
		//If the reservation has expired return false. Will remove the reservation elsewhere
		return false; 
	}
	if (!Res->Claimer.IsValid()) return false;
	//In case of the reserver being the one checking if claimed.
	return Res->Claimer.Get() != Claimer;
}

ESAS_ResourceValidity USAS_ResourceManagerComponent::CheckValidity(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData, AActor* Claimer) const
{
	if (!Key.IsValid() || !TypeData) return ESAS_ResourceValidity::InvalidKey;

	if (GetRemaining(Key, TypeData) <= 0)
	{
		return ESAS_ResourceValidity::Depleted;
	}

	if (Claimer && IsReservedByOther(Key, Claimer))
	{
		return ESAS_ResourceValidity::ReservedByOther;
	}

	return ESAS_ResourceValidity::Valid;

}

int32 USAS_ResourceManagerComponent::ApplyHarvest(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData, int32 RequestedAmount, USAS_ResourceClusterComponent* ClusterForVisuals, UPrimitiveComponent* HitComponentForVisuals)
{
	//TODO: Should probably move the requested amount to something in the Resource TypeData

	if (!Key.IsValid() || !TypeData) return 0;
	if (RequestedAmount <= 0) return 0;

	const int32 Current = GetRemaining(Key, TypeData);
	if (Current <= 0) return 0;

	const int32 Taken = FMath::Min(Current, RequestedAmount);
	const int32 NewRemaining = Current - Taken;

	FSAS_ResourceRuntimeState& State = FindOrAddState_OnModify(Key, TypeData);
	State.Remaining = NewRemaining;
	State.bDepleted = (NewRemaining <= 0);

	if (State.bDepleted)
	{
		Reservations.Remove(Key);
		if (ClusterForVisuals && HitComponentForVisuals)
		{
			ClusterForVisuals->MarkDepleted(HitComponentForVisuals, Key.InstanceIndex);
		}
	}
	return Taken;
}


void USAS_ResourceManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

double USAS_ResourceManagerComponent::Now() const
{
	return FPlatformTime::Seconds();
}

void USAS_ResourceManagerComponent::CleanupExpiredReservations()
{
	const double T = Now();

	TArray<FSAS_ResourceKey> ToRemove;
	ToRemove.Reserve(Reservations.Num());

	for (const auto& Pair : Reservations)
	{
		const FSAS_ResourceReservationState& Res = Pair.Value;
		if (Res.ExpiresAtSeconds > 0.0 && Res.ExpiresAtSeconds <= T)
		{
			ToRemove.Add(Pair.Key);
		}
	}

	for (const FSAS_ResourceKey& Key : ToRemove)
	{
		Reservations.Remove(Key);
	}
}

FSAS_ResourceRuntimeState& USAS_ResourceManagerComponent::FindOrAddState_OnModify(const FSAS_ResourceKey& Key, const USAS_ResourceTypeData* TypeData)
{
	FSAS_ResourceRuntimeState* Existing = ModifiedStates.Find(Key);
	if (Existing) return *Existing;

	FSAS_ResourceRuntimeState NewState;
	NewState.TypeData = const_cast<USAS_ResourceTypeData*>(TypeData);
	NewState.Remaining = TypeData ? TypeData->StartingAmount : 0;
	NewState.bDepleted = (NewState.Remaining <= 0);

	return ModifiedStates.Add(Key, NewState);

}


