// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_ResourceClusterComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Core/Components/SAS_ResourceManagerComponent.h"
#include "GameFramework/GameStateBase.h"


USAS_ResourceClusterComponent::USAS_ResourceClusterComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

FSAS_ResourceKey USAS_ResourceClusterComponent::MakeKey(const UPrimitiveComponent* HISMPrimitiveComponent, int32 InstanceIndex) const
{
	if (!HISMPrimitiveComponent) return FSAS_ResourceKey();

	const UHierarchicalInstancedStaticMeshComponent* HISM = AsHISM(HISMPrimitiveComponent);
	if (!HISM) return FSAS_ResourceKey();

	const FGuid* GuidPtr = ClusterGuids.Find(HISM);
	if (!GuidPtr) return FSAS_ResourceKey();

	return FSAS_ResourceKey(*GuidPtr, InstanceIndex);
}

USAS_ResourceTypeData* USAS_ResourceClusterComponent::GetTypeForHitComponent(const UPrimitiveComponent* HitComponent) const
{
	const UHierarchicalInstancedStaticMeshComponent* HISM = AsHISM(HitComponent);
	if (!HISM) return nullptr;

	const TObjectPtr<USAS_ResourceTypeData>* Found = ComponentTypeMap.Find(const_cast<UHierarchicalInstancedStaticMeshComponent*>(HISM));
	return Found ? Found->Get() : nullptr;
}

bool USAS_ResourceClusterComponent::IsInstanceInteractable(const UPrimitiveComponent* HitComponent, int32 InstanceIndex) const
{
	const UHierarchicalInstancedStaticMeshComponent* HISM = AsHISM(HitComponent);
	if (!HISM) return false;

	//In case this request is sent to the wrong component somehow. It should always be registered
	if (!ComponentTypeMap.Contains(const_cast<UHierarchicalInstancedStaticMeshComponent*>(HISM))) return false;

	return !IsDisabled(HISM, InstanceIndex);
}

void USAS_ResourceClusterComponent::MarkDepleted(UPrimitiveComponent* HitComponent, int32 InstanceIndex)
{
	UHierarchicalInstancedStaticMeshComponent* HISM = AsHISM(HitComponent);
	if (!HISM) return;

	//In case this request is sent to the wrong component somehow. It should always be registered
	if (!ComponentTypeMap.Contains(HISM)) return;

	DisableInstance(HISM, InstanceIndex);
	HideInstance(HISM, InstanceIndex);
	//TODO: Need to look into the best method to handle the collision using a HISM. We are moving it out of the playable space which should make this fine, but will follow up. Also need to look into updating the NavComponent.

}

bool USAS_ResourceClusterComponent::GetInstanceTransform(const UPrimitiveComponent* HitComponent, int32 InstanceIndex, FTransform& OutWorldTransform) const
{
	const UHierarchicalInstancedStaticMeshComponent* HISM = AsHISM(HitComponent);
	if (!HISM) return false;
	if (InstanceIndex == INDEX_NONE) return false;

	if (IsDisabled(HISM, InstanceIndex)) return false;

	return HISM->GetInstanceTransform(InstanceIndex, OutWorldTransform, true);
}

void USAS_ResourceClusterComponent::BeginPlay()
{
	Super::BeginPlay();

	FindActorHISMs();

}

void USAS_ResourceClusterComponent::FindActorHISMs()
{

	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UHierarchicalInstancedStaticMeshComponent*> HISMs;
	Owner->GetComponents(HISMs);

	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	USAS_ResourceManagerComponent* RM = GS ? GS->FindComponentByClass<USAS_ResourceManagerComponent>() : nullptr;

	for (UHierarchicalInstancedStaticMeshComponent* HISM : HISMs)
	{
		if (!HISM) continue;

		//TODO: Need to update this so we can have multipe types, but will handle later.
		USAS_ResourceTypeData* TypeData = DefaultTypeData;

		if (!TypeData) continue;
			
		ComponentTypeMap.FindOrAdd(HISM) = TypeData;
		EnsureGuid(HISM);

		if (RM)
		{
			RM->RegisterHISMToGrid(TypeData, this, HISM);
		}
	}
}

void USAS_ResourceClusterComponent::EnsureGuid(UHierarchicalInstancedStaticMeshComponent* HISM)
{
	if (!HISM) return;
	if (!ClusterGuids.Contains(HISM))
	{
		const FGuid NewGuid = FGuid::NewGuid();
		ClusterGuids.Add(HISM, NewGuid);
	}
}

const UHierarchicalInstancedStaticMeshComponent* USAS_ResourceClusterComponent::AsHISM(const UPrimitiveComponent* Comp) const
{
	return Cast<UHierarchicalInstancedStaticMeshComponent>(Comp);
}

UHierarchicalInstancedStaticMeshComponent* USAS_ResourceClusterComponent::AsHISM(UPrimitiveComponent* Comp) const
{
	return Cast<UHierarchicalInstancedStaticMeshComponent>(Comp);
}

void USAS_ResourceClusterComponent::HideInstance(UHierarchicalInstancedStaticMeshComponent* HISM, int32 InstanceIndex)
{
	if (!HISM) return;
	if (InstanceIndex == INDEX_NONE) return;

	FTransform Transform;
	if (!HISM->GetInstanceTransform(InstanceIndex, Transform, true))
	{
		return;
	}

	FVector Loc = Transform.GetLocation();
	Loc.Z -= 10000.f;

	Transform.SetLocation(Loc);

	HISM->UpdateInstanceTransform(InstanceIndex, Transform, true, true, true);
}

void USAS_ResourceClusterComponent::DisableInstance(UHierarchicalInstancedStaticMeshComponent* HISM, int32 InstanceIndex)
{
	if (!HISM) return;
	if (InstanceIndex == INDEX_NONE) return;

	TSet<int32>& SetRef = DisabledInstancesByComponent.FindOrAdd(HISM);
	SetRef.Add(InstanceIndex);
}

bool USAS_ResourceClusterComponent::IsDisabled(const UHierarchicalInstancedStaticMeshComponent* HISM, int32 InstanceIndex) const
{
	if (!HISM) return true;

	const TSet<int32>* SetPtr = DisabledInstancesByComponent.Find(HISM);
	if (!SetPtr) return false;

	return SetPtr->Contains(InstanceIndex);
}


