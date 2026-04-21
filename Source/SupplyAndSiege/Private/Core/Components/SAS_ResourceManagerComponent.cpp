

#include "Core/Components/SAS_ResourceManagerComponent.h"
#include "Misc/DataAssets/SAS_ResourceTypeData.h"
#include "Core/Components/SAS_ResourceClusterComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
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
		ReleaseAllReservationsForClaimer(Claimer, FSAS_ResourceKey(), false);
		FSAS_ResourceReservationState NewRes;
		NewRes.Claimer = Claimer;
		NewRes.ExpiresAtSeconds = Exp;
		Reservations.Add(Key, NewRes);
		
		return true;
	}

	if (Existing->Claimer.IsValid() && Existing->Claimer.Get() == Claimer)
	{
		Existing->ExpiresAtSeconds = Exp;
		return true;
	}

	const double T = Now();
	if (Existing->Claimer.IsValid() && (Existing->ExpiresAtSeconds == 0.0 || Existing->ExpiresAtSeconds > T)) return false;

	//If the existing claimer is no longer valid or the time has expired.

	ReleaseAllReservationsForClaimer(Claimer, FSAS_ResourceKey(), false);

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

int32 USAS_ResourceManagerComponent::RegisterISMToGrid(const USAS_ResourceTypeData* ResourceType, const USAS_ResourceClusterComponent* Cluster, UInstancedStaticMeshComponent* ISM)
{
	if (!ResourceType || !Cluster || !ISM) return 0;

	FSAS_SpatialGrid& Grid = GridsByType.FindOrAdd(ResourceType);

	//Grid size CANNOT be 0. If we find 0 force the default of 500.
	if (Grid.CellSize <= 0.f)
	{
		const float* SizePtr = ResourceTypeGridSize.Find(ResourceType);
		Grid.CellSize = (SizePtr && *SizePtr > 0.f) ? *SizePtr : 500.f;
	}

	const int32 Count = ISM->GetInstanceCount();
	if (Count <= 0) return 0;

	int32 Registered = 0;
	for (int32 InstanceIndex = 0; InstanceIndex < Count; ++InstanceIndex)
	{
		FTransform Xform;
		if (!ISM->GetInstanceTransform(InstanceIndex, Xform, true)) continue;

		const FVector WorldLoc = Xform.GetLocation();
		const FSAS_ResourceKey Key = Cluster->MakeKey(ISM, InstanceIndex);
		if (Key.InstanceIndex == INDEX_NONE || !Key.ClusterGuid.IsValid()) continue;
		if (KeyToHandle.Contains(Key)) continue;

		const FIntPoint Cell = WorldToCell2D(WorldLoc, Grid.CellSize);
		Grid.Cells.FindOrAdd(Cell).Add(Key);

		FSAS_ISMHandle Handle;
		Handle.ISM = ISM;
		Handle.InstanceIndex = InstanceIndex;
		Handle.WorldLocation = WorldLoc;

		KeyToHandle.Add(Key, Handle);
		++Registered;
	}

	return Registered;

}

void USAS_ResourceManagerComponent::GetAvailableResourceLocationsInRadius(const USAS_ResourceTypeData* ResourceType, const FVector WorldLocation, float DesiredSearchRadius, TArray<FVector>& OutResourceLocations, AActor* Claimer) const
{
	OutResourceLocations.Reset();
	if (!ResourceType) return;

	const FSAS_SpatialGrid* Grid = GridsByType.Find(ResourceType);
	if (!Grid) return;

	if (Grid->CellSize <= 0.f) return;

	const FIntPoint CenterCell = WorldToCell2D(WorldLocation, Grid->CellSize);
	const int32 CellRadius = FMath::Max(0, FMath::CeilToInt(DesiredSearchRadius / Grid->CellSize));

	for (int32 X = CenterCell.X - CellRadius; X <= CenterCell.X + CellRadius; ++X)
	{
		for (int32 Y = CenterCell.Y - CellRadius; Y <= CenterCell.Y + CellRadius; ++Y)
		{
			const FIntPoint Cell(X, Y);
			const TArray<FSAS_ResourceKey>* KeysInCell = Grid->Cells.Find(Cell);
			if (!KeysInCell) continue;

			for (const FSAS_ResourceKey& Key : *KeysInCell)
			{
				const FSAS_ISMHandle* Handle = KeyToHandle.Find(Key);
				if (!Handle) continue;
				if (!Handle->ISM.IsValid()) continue;
				if (Handle->InstanceIndex == INDEX_NONE) continue;

				if (CheckValidity(Key, ResourceType, Claimer) != ESAS_ResourceValidity::Valid) continue;
				OutResourceLocations.Add(Handle->WorldLocation);
			}
		}
	}
}

bool USAS_ResourceManagerComponent::TryReserveResourceFromEQSLocation(const USAS_ResourceTypeData* ResourceType, const FVector WorldLocation, float DesiredSearchRadius, AActor* Claimer, FSAS_ResourceKey& OutKey, FVector& OutResourceLocation, float DurationSeconds)
{
	
	OutKey = FSAS_ResourceKey();
	OutResourceLocation = FVector::ZeroVector;

	if (!ResourceType || !Claimer) return false;

	CleanupExpiredReservations();

	const FSAS_SpatialGrid* Grid = GridsByType.Find(ResourceType);
	if (!Grid) return false;
	if (Grid->CellSize <= 0.f) return false;

	const FIntPoint CenterCell = WorldToCell2D(WorldLocation, Grid->CellSize);
	const int32 CellRadius = FMath::Max(0, FMath::CeilToInt(DesiredSearchRadius / Grid->CellSize));

	for (int32 X = CenterCell.X - CellRadius; X <= CenterCell.X + CellRadius; ++X)
	{
		for (int32 Y = CenterCell.Y - CellRadius; Y <= CenterCell.Y + CellRadius; ++Y)
		{
			const FIntPoint Cell(X, Y);
			const TArray<FSAS_ResourceKey>* KeysInCell = Grid->Cells.Find(Cell);
			if (!KeysInCell) continue;

			for (const FSAS_ResourceKey& Key : *KeysInCell)
			{
				const FSAS_ISMHandle* Handle = KeyToHandle.Find(Key);
				if (!Handle) continue;
				if (!Handle->ISM.IsValid()) continue;

				if (CheckValidity(Key, ResourceType, Claimer) != ESAS_ResourceValidity::Valid) continue;
				if (!TryReserve(Key, Claimer, DurationSeconds)) continue;
				OutKey = Key;
				OutResourceLocation = Handle->WorldLocation;
				return true;
			}
		}
	}
	return false;
}

bool USAS_ResourceManagerComponent::GetResourceTransform(const FSAS_ResourceKey& Key, FTransform& OutWorldTransform)
{
	if (!Key.IsValid()) return false;

	const FSAS_ISMHandle* Handle = KeyToHandle.Find(Key);
	if (!Handle) return false;
	if (!Handle->ISM.IsValid()) return false;

	return Handle->ISM->GetInstanceTransform(Handle->InstanceIndex, OutWorldTransform, true);

}

void USAS_ResourceManagerComponent::ReleaseAllReservationsForClaimer(AActor* Claimer, const FSAS_ResourceKey& KeyToKeep, bool bHasKeyToKeep)
{
	if (!Claimer) return;

	TArray<FSAS_ResourceKey> KeysToRemove;
	KeysToRemove.Reserve(Reservations.Num());

	for (const TPair<FSAS_ResourceKey, FSAS_ResourceReservationState>& Pair : Reservations)
	{
		const FSAS_ResourceKey& ExistingKey = Pair.Key;
		const FSAS_ResourceReservationState& Res = Pair.Value;

		if (!Res.Claimer.IsValid()) continue;
		if (Res.Claimer.Get() != Claimer) continue;

		if (bHasKeyToKeep && ExistingKey == KeyToKeep) continue;

		KeysToRemove.Add(ExistingKey);
	}
	for (const FSAS_ResourceKey& Key : KeysToRemove)
	{
		Reservations.Remove(Key);
	}
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

FIntPoint USAS_ResourceManagerComponent::WorldToCell2D(const FVector& World, float CellSize) const
{
	return FIntPoint(
	FMath::FloorToInt(World.X / CellSize),
	FMath::FloorToInt(World.Y / CellSize)
	);
}



