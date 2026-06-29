#include "Core/Components/SAS_UnitManagerComponent.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "Iris/ReplicationSystem/NetObjectFactory.h"
#include "Core/Interfaces/SAS_ClickTarget.h"
#include "Core/Components/SAS_ResourceClusterComponent.h"
#include "Misc/DataAssets/SAS_UnitTypeData.h"
#include "Core/Components/SAS_UnitControlComponent.h"


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
		
	}
}

void USAS_UnitManagerComponent::SetSelectedUnits(const TArray<USAS_UnitInformationComponent*>& NewSelection)
{
	TArray<TWeakObjectPtr<USAS_UnitInformationComponent>> CleanNewSelection;
	CleanNewSelection.Reserve(NewSelection.Num());

	for (USAS_UnitInformationComponent* UnitInfo : NewSelection)
	{
		if (!IsValid(UnitInfo)) continue;
		CleanNewSelection.AddUnique(UnitInfo);
	}

	bool bChanged = SelectedUnits.Num() != CleanNewSelection.Num();

	if (!bChanged)
	{
		for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfo : CleanNewSelection)
		{
			if (!SelectedUnits.Contains(UnitInfo))
			{
				bChanged = true;
				break;
			}
		}
	}

	if (!bChanged) return;

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfo : SelectedUnits)
	{
		if (UnitInfo.IsValid() && !CleanNewSelection.Contains(UnitInfo))
		{
			UnitInfo->NotifyDeselected(AssignedTeam);
		}
	}

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfo : CleanNewSelection)
	{
		if (UnitInfo.IsValid() && !SelectedUnits.Contains(UnitInfo))
		{
			UnitInfo->NotifySelected(AssignedTeam);
		}
	}

	SelectedUnits = CleanNewSelection;
	OnUnitSelectionChange.Broadcast(SelectedUnits);
}

void USAS_UnitManagerComponent::ClearAllSelectedUnits()
{
	TArray<USAS_UnitInformationComponent*> EmptySelection;
	SetSelectedUnits(EmptySelection);
}

void USAS_UnitManagerComponent::AddSelectedUnit(USAS_UnitInformationComponent* UnitInfo)
{
	if (!IsValid(UnitInfo)) return;

	TArray<USAS_UnitInformationComponent*> NewSelection;
	NewSelection.Reserve(SelectedUnits.Num() + 1);

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& SelectedUnit : SelectedUnits)
	{
		if (SelectedUnit.IsValid())
		{
			NewSelection.Add(SelectedUnit.Get());
		}
	}

	NewSelection.AddUnique(UnitInfo);
	SetSelectedUnits(NewSelection);
}

void USAS_UnitManagerComponent::RightClickReceived(const FHitResult Hit)
{
	if (SelectedUnits.Num() == 0) return;

	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor))
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	if (!HitActor->GetClass()->ImplementsInterface(USAS_ClickTarget::StaticClass()))
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	const ESAS_ClickTargetType HitActorType = ISAS_ClickTarget::Execute_GetClickTargetType(HitActor);

	switch (HitActorType)
	{
	case ESAS_ClickTargetType::Ground:
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);

		break;
	case ESAS_ClickTargetType::Resource:
		HandleResourceRightClickForSelectedUnits(Hit);
		break;

	case ESAS_ClickTargetType::Unit:
	{

		USAS_UnitInformationComponent* TargetUnitInformationComponent = HitActor->FindComponentByClass<USAS_UnitInformationComponent>();

		if (!IsValid(TargetUnitInformationComponent))
		{
			HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
			return;
		}

		if (TargetUnitInformationComponent->GetTeam() == AssignedTeam)
		{
			HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
			return;
		}

		HandleEnemyUnitRightClickForSelectedUnits(TargetUnitInformationComponent, Hit.ImpactPoint);
		break;
	}

	default:
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);

		break;
	}

}

void USAS_UnitManagerComponent::IssueMoveOrderToUnits(const TArray<TWeakObjectPtr<USAS_UnitControlComponent>>& UnitsToMove, const FVector& WorldLocation)
{
	if (UnitsToMove.Num() == 0) return;

	if (UnitsToMove.Num() == 1)
	{
		if (UnitsToMove[0].IsValid())
		{
			UnitsToMove[0]->HandleGroundRightClick(WorldLocation);
		}
		return;
	}

	if (ensureMsgf(FormationQuery, TEXT("Please assign an EQS to %s's UnitManagerComponent's FormationQuery"), *GetNameSafe(GetOwner())))
	{
		PendingFormationUnits = UnitsToMove;

		AActor* WorldContextObject = GetOwner();
		LastRightClickLocation = WorldLocation;
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
void USAS_UnitManagerComponent::OnFormationQueryComplete(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Formation Query failed!"));
		return;
	}


	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.Num() == 0)
	{
		PendingFormationUnits.Empty();
		return;
	}

	int32 LocIndex = 0;

	for (const TWeakObjectPtr<USAS_UnitControlComponent>& UnitCompPtr : PendingFormationUnits)
	{
		if (UnitCompPtr.IsValid())
		{
			UnitCompPtr->HandleGroundRightClick(Locations[LocIndex]);
			LocIndex = (LocIndex + 1) % Locations.Num();
		}
	}

	PendingFormationUnits.Empty();
}
UE_ENABLE_OPTIMIZATION

USAS_UnitControlComponent* USAS_UnitManagerComponent::GetUnitControlFromInfo(USAS_UnitInformationComponent* UnitInfo) const
{
	if (!UnitInfo) return nullptr;

	AActor* Owner = UnitInfo->GetOwner();
	if (!Owner) return nullptr;

	return Owner->FindComponentByClass<USAS_UnitControlComponent>();
}

void USAS_UnitManagerComponent::HandleGroundRightClickForSelectedUnits(const FVector& WorldLocation)
{
	TArray<TWeakObjectPtr<USAS_UnitControlComponent>> UnitsRequiringMove;
	UnitsRequiringMove.Reserve(SelectedUnits.Num());

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfoPtr : SelectedUnits)
	{
		if (!UnitInfoPtr.IsValid()) continue;

		USAS_UnitControlComponent* UnitControl = GetUnitControlFromInfo(UnitInfoPtr.Get());
		if (!UnitControl) continue;

		if (UnitControl->GroundSelectIsMove())
		{
			UnitsRequiringMove.Add(UnitControl);
		}
		else
		{
			UnitControl->HandleGroundRightClick(WorldLocation);
		}
	}
	if (UnitsRequiringMove.Num() > 0)
	{
		IssueMoveOrderToUnits(UnitsRequiringMove, WorldLocation);
	}
}

void USAS_UnitManagerComponent::HandleResourceRightClickForSelectedUnits(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	UPrimitiveComponent* HitComp = Hit.GetComponent();
	const int32 InstanceIndex = Hit.Item;

	if (InstanceIndex == INDEX_NONE)
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	USAS_ResourceClusterComponent* Cluster = HitActor->FindComponentByClass<USAS_ResourceClusterComponent>();
	if (!Cluster)
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	USAS_ResourceTypeData* HitResourceType = Cluster->GetTypeForHitComponent(HitComp);
	if (!HitResourceType)
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	FTransform InstanceTransform;
	if (!Cluster->GetInstanceTransform(HitComp, InstanceIndex, InstanceTransform))
	{
		HandleGroundRightClickForSelectedUnits(Hit.ImpactPoint);
		return;
	}

	const FSAS_ResourceKey HitResourceKey = Cluster->MakeKey(HitComp, InstanceIndex);

	TArray<TWeakObjectPtr<USAS_UnitControlComponent>> UnitsRequiringMove;
	UnitsRequiringMove.Reserve(SelectedUnits.Num());

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfoPtr : SelectedUnits)
	{
		if (!UnitInfoPtr.IsValid()) continue;

		USAS_UnitControlComponent* UnitControl = GetUnitControlFromInfo(UnitInfoPtr.Get());
		if (!UnitControl) continue;

		if (UnitControl->ResourceSelectIsMove(HitResourceType))
		{
			UnitsRequiringMove.Add(UnitControl);
		}
		else
		{
			UnitControl->HandleResourceRightClick(HitResourceType, HitResourceKey, InstanceTransform.GetLocation());
		}
	}

	if (UnitsRequiringMove.Num() > 0)
	{
		IssueMoveOrderToUnits(UnitsRequiringMove, InstanceTransform.GetLocation());
	}

}

void USAS_UnitManagerComponent::HandleEnemyUnitRightClickForSelectedUnits(USAS_UnitInformationComponent* TargetUnitInformationComponent, const FVector& ClickLocation)
{
	if (!IsValid(TargetUnitInformationComponent)) return;

	TArray<TWeakObjectPtr<USAS_UnitControlComponent>> UnitsRequiringMove;
	UnitsRequiringMove.Reserve(SelectedUnits.Num());

	for (const TWeakObjectPtr<USAS_UnitInformationComponent>& UnitInfoPtr : SelectedUnits)
	{
		if (!UnitInfoPtr.IsValid()) continue;

		USAS_UnitControlComponent* UnitControl = GetUnitControlFromInfo(UnitInfoPtr.Get());
		if (!UnitControl) continue;

		if (!UnitControl->CanReceiveAttackOrder())
		{
			UnitsRequiringMove.Add(UnitControl);
			continue;
		}

		UnitControl->HandleAttackUnitOrder(TargetUnitInformationComponent, ClickLocation);
	}

	if (UnitsRequiringMove.Num() > 0)
	{
		IssueMoveOrderToUnits(UnitsRequiringMove, ClickLocation);
	}
}

