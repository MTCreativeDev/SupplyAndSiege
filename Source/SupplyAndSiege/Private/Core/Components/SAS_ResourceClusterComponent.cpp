// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/SAS_ResourceClusterComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Core/Components/SAS_ResourceManagerComponent.h"
#include "GameFramework/GameStateBase.h"


USAS_ResourceClusterComponent::USAS_ResourceClusterComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

FSAS_ResourceKey USAS_ResourceClusterComponent::MakeKey(const UPrimitiveComponent* ISMPrimitiveComponent, int32 InstanceIndex) const
{
	if (!ISMPrimitiveComponent) return FSAS_ResourceKey();

	const UInstancedStaticMeshComponent* ISM = AsISM(ISMPrimitiveComponent);
	if (!ISM) return FSAS_ResourceKey();

	const FGuid* GuidPtr = ClusterGuids.Find(ISM);
	if (!GuidPtr) return FSAS_ResourceKey();

	return FSAS_ResourceKey(*GuidPtr, InstanceIndex);
}

USAS_ResourceTypeData* USAS_ResourceClusterComponent::GetTypeForHitComponent(const UPrimitiveComponent* HitComponent) const
{
	const UInstancedStaticMeshComponent* ISM = AsISM(HitComponent);
	if (!ISM) return nullptr;

	const TObjectPtr<USAS_ResourceTypeData>* Found = ComponentTypeMap.Find(const_cast<UInstancedStaticMeshComponent*>(ISM));
	return Found ? Found->Get() : nullptr;
}

bool USAS_ResourceClusterComponent::IsInstanceInteractable(const UPrimitiveComponent* HitComponent, int32 InstanceIndex) const
{
	const UInstancedStaticMeshComponent* ISM = AsISM(HitComponent);
	if (!ISM) return false;

	//In case this request is sent to the wrong component somehow. It should always be registered
	if (!ComponentTypeMap.Contains(const_cast<UInstancedStaticMeshComponent*>(ISM))) return false;

	return !IsDisabled(ISM, InstanceIndex);
}

void USAS_ResourceClusterComponent::MarkDepleted(UPrimitiveComponent* HitComponent, int32 InstanceIndex)
{
	UInstancedStaticMeshComponent* ISM = AsISM(HitComponent);
	if (!ISM) return;

	//In case this request is sent to the wrong component somehow. It should always be registered
	if (!ComponentTypeMap.Contains(ISM)) return;

	DisableInstance(ISM, InstanceIndex);
	HideInstance(ISM, InstanceIndex);
	//TODO: Need to look into the best method to handle the collision using a HISM. We are moving it out of the playable space which should make this fine, but will follow up. Also need to look into updating the NavComponent.

}

bool USAS_ResourceClusterComponent::GetInstanceTransform(const UPrimitiveComponent* HitComponent, int32 InstanceIndex, FTransform& OutWorldTransform) const
{
	const UInstancedStaticMeshComponent* ISM = AsISM(HitComponent);
	if (!ISM) return false;
	if (InstanceIndex == INDEX_NONE) return false;

	if (IsDisabled(ISM, InstanceIndex)) return false;

	return ISM->GetInstanceTransform(InstanceIndex, OutWorldTransform, true);
}

void USAS_ResourceClusterComponent::BeginPlay()
{
	Super::BeginPlay();

	FindActorISMs();

}

void USAS_ResourceClusterComponent::FindActorISMs()
{

	AActor* Owner = GetOwner();
	if (!Owner) return;

	TArray<UInstancedStaticMeshComponent*> ISMs;
	Owner->GetComponents(ISMs);

	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	USAS_ResourceManagerComponent* RM = GS ? GS->FindComponentByClass<USAS_ResourceManagerComponent>() : nullptr;

	for (UInstancedStaticMeshComponent* ISM : ISMs)
	{
		if (!ISM) continue;

		USAS_ResourceTypeData* TypeData = CheckComponentForResourceTag(ISM);

		if (!TypeData) continue;
			
		ComponentTypeMap.FindOrAdd(ISM) = TypeData;
		EnsureGuid(ISM);

		if (RM)
		{
			RM->RegisterISMToGrid(TypeData, this, ISM);
		}
	}
}

void USAS_ResourceClusterComponent::EnsureGuid(UInstancedStaticMeshComponent* ISM)
{
	if (!ISM) return;
	if (!ClusterGuids.Contains(ISM))
	{
		const FGuid NewGuid = FGuid::NewGuid();
		ClusterGuids.Add(ISM, NewGuid);
	}
}

const UInstancedStaticMeshComponent* USAS_ResourceClusterComponent::AsISM(const UPrimitiveComponent* Comp) const
{
	return Cast<UInstancedStaticMeshComponent>(Comp);
}

UInstancedStaticMeshComponent* USAS_ResourceClusterComponent::AsISM(UPrimitiveComponent* Comp) const
{
	return Cast<UInstancedStaticMeshComponent>(Comp);
}

void USAS_ResourceClusterComponent::HideInstance(UInstancedStaticMeshComponent* ISM, int32 InstanceIndex)
{
	if (!ISM) return;
	if (InstanceIndex == INDEX_NONE) return;

	FTransform Transform;
	if (!ISM->GetInstanceTransform(InstanceIndex, Transform, true))
	{
		return;
	}

	FVector Loc = Transform.GetLocation();
	Loc.Z -= 10000.f;

	Transform.SetLocation(Loc);

	ISM->UpdateInstanceTransform(InstanceIndex, Transform, true, true, true);
}

void USAS_ResourceClusterComponent::DisableInstance(UInstancedStaticMeshComponent* ISM, int32 InstanceIndex)
{
	if (!ISM) return;
	if (InstanceIndex == INDEX_NONE) return;

	TSet<int32>& SetRef = DisabledInstancesByComponent.FindOrAdd(ISM);
	SetRef.Add(InstanceIndex);
}

bool USAS_ResourceClusterComponent::IsDisabled(const UInstancedStaticMeshComponent* ISM, int32 InstanceIndex) const
{
	if (!ISM) return true;

	const TSet<int32>* SetPtr = DisabledInstancesByComponent.Find(ISM);
	if (!SetPtr) return false;

	return SetPtr->Contains(InstanceIndex);
}

USAS_ResourceTypeData* USAS_ResourceClusterComponent::CheckComponentForResourceTag(const UPrimitiveComponent* Comp) const
{
	if (!Comp) return nullptr;

	for (const TPair<FName, TObjectPtr<USAS_ResourceTypeData>>& Pair : TagToResourceType)
	{
		if (Comp->ComponentHasTag(Pair.Key)) return Pair.Value;
	}

	return nullptr;
}


