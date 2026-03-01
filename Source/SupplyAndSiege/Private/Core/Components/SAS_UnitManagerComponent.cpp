#include "Core/Components/SAS_UnitManagerComponent.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "Iris/ReplicationSystem/NetObjectFactory.h"
#include "Core/Interfaces/SAS_ClickTarget.h"
#include "Core/Components/SAS_ResourceClusterComponent.h"
#include "Misc/DataAssets/SAS_UnitTypeData.h"


// Sets default values for this component's properties
USAS_UnitManagerComponent::USAS_UnitManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_UnitManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USAS_UnitManagerComponent::AssignSelectableUnit(TWeakObjectPtr<AActor> NewUnit, bool BypassComponentCheck)
{
	if (!NewUnit.IsValid()) return;

	USAS_UnitInformationComponent* UnitInformationComponent = nullptr;

	if (!BypassComponentCheck)
	{
		//Only accepts units that have the UnitInformationComponent. The bool input is used if the component is the thing adding the unit.
		AActor* Actor = NewUnit.Get();
		UnitInformationComponent = Actor->FindComponentByClass<USAS_UnitInformationComponent>();
		if (!UnitInformationComponent) return;
	}

	//Only accepts units that have the UnitInformationComponent
	SelectableUnits.AddUnique(NewUnit);
}

void USAS_UnitManagerComponent::SetTeam(ESAS_Team NewTeam)
{
	AssignedTeam = NewTeam;
}

void USAS_UnitManagerComponent::RemoveSelectableUnit(TWeakObjectPtr<AActor> UnitToRemove)
{
	SelectableUnits.Remove(UnitToRemove);

	//TODO:: Need to set it up so that when a unit is removed it is removed from any current unit selections.


	//Debug print
	if (GEngine)
	{
		FString ActorList;

		for (const TWeakObjectPtr<AActor>& Unit : SelectableUnits)
		{
			if (Unit.IsValid())
			{
				ActorList += Unit->GetName();
				ActorList += TEXT(", ");
			}
		}

		GEngine->AddOnScreenDebugMessage(
			-1,
			30.f,
			FColor::Green,
			FString::Printf(
				TEXT("SelectableUnits Count: %d | [%s]"),
				SelectableUnits.Num(),
				*ActorList
			)
		);
	}
}

void USAS_UnitManagerComponent::AddSelectedUnit(TWeakObjectPtr<USAS_UnitInformationComponent> UnitInformation)
{
	if (!UnitInformation.IsValid()) return;
	SelectedUnits.AddUnique(UnitInformation);

	UnitInformation->NotifySelected(AssignedTeam);
	OnUnitSelectionChange.Broadcast(SelectedUnits);
}

void USAS_UnitManagerComponent::RemoveSelectedUnit(TWeakObjectPtr<USAS_UnitInformationComponent> UnitInformation)
{
	SelectedUnits.Remove(UnitInformation);
	UnitInformation->NotifyDeselected(AssignedTeam);
	OnUnitSelectionChange.Broadcast(SelectedUnits);
}

void USAS_UnitManagerComponent::ClearAllSelectedUnits()
{
	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitCompPtr : SelectedUnits)
	{
		if (!UnitCompPtr.IsValid()) continue;
		UnitCompPtr->NotifyDeselected(AssignedTeam);
	}
	SelectedUnits.Empty();
	OnUnitSelectionChange.Broadcast(SelectedUnits);
}

void USAS_UnitManagerComponent::RightClickReceived(const FHitResult Hit)
{


	AActor* HitActor = Hit.GetActor();
	if (!HitActor) return;

	if (HitActor->GetClass()->ImplementsInterface(USAS_ClickTarget::StaticClass()))
	{
		ESAS_ClickTargetType HitActorType = ISAS_ClickTarget::Execute_GetClickTargetType(HitActor);
	
		TArray<TWeakObjectPtr<USAS_UnitInformationComponent>> MoveOnlyUnits;
		MoveOnlyUnits.Reserve(SelectedUnits.Num());

		switch (HitActorType)
		{
		case ESAS_ClickTargetType::Ground:
			if (SelectedUnits.Num() <= 0) break;
			IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);
			//Arguably should filter out buildings, but they won't have any functionality for a move order anyway.
			break;

		case ESAS_ClickTargetType::Resource:
		{
			UPrimitiveComponent* HitComp = Hit.GetComponent();
			const int32 InstanceIndex = Hit.Item;

			if (InstanceIndex == INDEX_NONE)
			{
				if (SelectedUnits.Num() > 0)
				{
					IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);
				}
				break;
			}

			USAS_ResourceClusterComponent* Cluster = HitActor->FindComponentByClass<USAS_ResourceClusterComponent>();

			//If we run into an issue just move to location
			if (!Cluster)
			{
				if (SelectedUnits.Num() <= 0) break;
				IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);
				break;
			}

			USAS_ResourceTypeData* HitResourceType = Cluster->GetTypeForHitComponent(HitComp);

			//If we run into an issue just move to location
			if (!HitResourceType)
			{
				if (SelectedUnits.Num() <= 0) break;
				IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);
				break;
			}

			FGuid ClusterGuid = Cluster->ClusterGuid;
			FSAS_ResourceKey HitResourceKey = FSAS_ResourceKey(ClusterGuid, InstanceIndex);

			for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitCompPtr : SelectedUnits)
			{
				if (!UnitCompPtr.IsValid()) continue;
				USAS_UnitInformationComponent* UnitComp = UnitCompPtr.Get();
				if (!UnitComp) continue;

				const USAS_UnitTypeData* UnitType = UnitComp->UnitType;
				if (!UnitType) continue;

				const ESAS_UnitCategory CurrentUnitCategory = UnitType->UnitCategory;

				switch (CurrentUnitCategory)
				{
					case ESAS_UnitCategory::None: break;

					case ESAS_UnitCategory::Pawn_Villager:
					{
						UnitComp->IssueHarvestOrder(HitResourceType, HitResourceKey, Hit.ImpactPoint);
						break;
					}

					case ESAS_UnitCategory::Pawn_Courier:
						MoveOnlyUnits.Add(UnitCompPtr);
						break;

					case ESAS_UnitCategory::Pawn_Military: 
						MoveOnlyUnits.Add(UnitCompPtr);
						break;

					default: break;
				}

			}

			if (MoveOnlyUnits.Num() > 0)
			{
				IssueMoveOrderToUnits(MoveOnlyUnits, Hit.ImpactPoint);
			}

			break;
		}

		case ESAS_ClickTargetType::Unit:
			//TODO
			if (SelectedUnits.Num() <= 0) break;
			IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);
			//Arguably should filter out buildings, but they won't have any functionality for a move order anyway.
			break;

		default: 
			//TODO:
			if (SelectedUnits.Num() <= 0) break;
			IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);
			//Arguably should filter out buildings, but they won't have any functionality for a move order anyway.
			break;
		}
	}
	else
	{

		if (SelectedUnits.Num() >0)
		{
			IssueMoveOrderToSelectedUnits(Hit.ImpactPoint);

		}
	}
}

void USAS_UnitManagerComponent::IssueMoveOrderToUnits(const TArray<TWeakObjectPtr<USAS_UnitInformationComponent>>& UnitsToMove, const FVector& WorldLocation)
{
	if (UnitsToMove.Num() == 0) return;

	if (ensureMsgf(FormationQuery, TEXT("Please assign an EQS to %s's UnitManagerComponent's FormationQuery"), *GetNameSafe(GetOwner())))
	{
		PendingFormationUnits = UnitsToMove;

		AActor* WorldContextObject = GetOwner();
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(WorldContextObject, FormationQuery, GetOwner(), EEnvQueryRunMode::AllMatching, nullptr);

		constexpr float SpaceBetween = 100.f;
		QueryInstance->SetNamedParam(TEXT("SimpleGrid.SpaceBetween"), SpaceBetween);

		const int32 NumUnits = UnitsToMove.Num();
		const int32 PointLength = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(NumUnits)));
		const float GridSize = (PointLength - 1) * SpaceBetween;
		QueryInstance->SetNamedParam(TEXT("SimpleGrid.GridSize"), GridSize);

		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ThisClass::OnFormationQueryComplete);
	}

}



UE_DISABLE_OPTIMIZATION
void USAS_UnitManagerComponent::IssueMoveOrderToSelectedUnits(FVector WorldLocation)
{
	IssueMoveOrderToUnits(SelectedUnits, WorldLocation);
}

void USAS_UnitManagerComponent::OnFormationQueryComplete(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Formation Query failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	int32 LocIndex = 0;

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitCompPtr : PendingFormationUnits)
	{
		if (UnitCompPtr.IsValid())
		{
			UnitCompPtr->IssueMoveOrder(Locations[LocIndex]);
			LocIndex = (LocIndex + 1) % Locations.Num();
		}
	}

	PendingFormationUnits.Empty();
}
UE_ENABLE_OPTIMIZATION
