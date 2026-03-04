// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "SAS_ResourceClusterComponent.generated.h"

class USAS_ResourceTypeData;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_ResourceClusterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_ResourceClusterComponent();

	FSAS_ResourceKey MakeKey(const UPrimitiveComponent* HISMPrimitiveComponent, int32 InstanceIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	USAS_ResourceTypeData* GetTypeForHitComponent(const UPrimitiveComponent* HitComponent) const;

	bool IsInstanceInteractable(const UPrimitiveComponent* HitComponent, int32 InstanceIndex) const;

	void MarkDepleted(UPrimitiveComponent* HitComponent, int32 InstanceIndex);

	bool GetInstanceTransform(const UPrimitiveComponent* HitComponent, int32 InstanceIndex, FTransform& OutWorldTransform) const;


protected:
	virtual void BeginPlay() override;

	void FindActorHISMs();

private:

	void EnsureGuid(UHierarchicalInstancedStaticMeshComponent* HISM);
	const UHierarchicalInstancedStaticMeshComponent* AsHISM(const UPrimitiveComponent* Comp) const;
	UHierarchicalInstancedStaticMeshComponent* AsHISM(UPrimitiveComponent* Comp) const;

	//Will likely always use HideInstance and DisableInstance at the same time, but separate in case we run into edge cases.
	void HideInstance(UHierarchicalInstancedStaticMeshComponent* HISM, int32 InstanceIndex);
	void DisableInstance(UHierarchicalInstancedStaticMeshComponent* HISM, int32 InstanceIndex);
	bool IsDisabled(const UHierarchicalInstancedStaticMeshComponent* HISM, int32 InstanceIndex) const;

public:	

	UPROPERTY(BlueprintReadOnly, Category = "Resource")
	TMap<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>, TObjectPtr<USAS_ResourceTypeData>> ComponentTypeMap;

protected:

	//TEMPORARY
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TObjectPtr<USAS_ResourceTypeData> DefaultTypeData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	TMap<TObjectPtr<UHierarchicalInstancedStaticMeshComponent>, FGuid> ClusterGuids;

private:


	TMap<TObjectPtr<const UHierarchicalInstancedStaticMeshComponent>, TSet<int32>> DisabledInstancesByComponent;







};
