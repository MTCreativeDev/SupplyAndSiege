// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "SAS_ResourceClusterComponent.generated.h"

class USAS_ResourceTypeData;
class UInstancedStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_ResourceClusterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_ResourceClusterComponent();

	FSAS_ResourceKey MakeKey(const UPrimitiveComponent* ISMPrimitiveComponent, int32 InstanceIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Resource")
	USAS_ResourceTypeData* GetTypeForHitComponent(const UPrimitiveComponent* HitComponent) const;

	bool IsInstanceInteractable(const UPrimitiveComponent* HitComponent, int32 InstanceIndex) const;

	void MarkDepleted(UPrimitiveComponent* HitComponent, int32 InstanceIndex);

	bool GetInstanceTransform(const UPrimitiveComponent* HitComponent, int32 InstanceIndex, FTransform& OutWorldTransform) const;

	bool AreAllInstancesDepleted(const UInstancedStaticMeshComponent* ISM) const;

protected:
	virtual void BeginPlay() override;

	void FindActorISMs();

private:

	void EnsureGuid(UInstancedStaticMeshComponent* ISM);
	const UInstancedStaticMeshComponent* AsISM(const UPrimitiveComponent* Comp) const;
	UInstancedStaticMeshComponent* AsISM(UPrimitiveComponent* Comp) const;

	//Will likely always use HideInstance and DisableInstance at the same time, but separate in case we run into edge cases.
	void HideInstance(UInstancedStaticMeshComponent* ISM, int32 InstanceIndex);
	void DisableInstance(UInstancedStaticMeshComponent* ISM, int32 InstanceIndex);
	bool IsDisabled(const UInstancedStaticMeshComponent* ISM, int32 InstanceIndex) const;
	USAS_ResourceTypeData* CheckComponentForResourceTag(const UPrimitiveComponent* Comp) const;

public:	

	UPROPERTY(VisibleAnywhere, Category = "Resource")
	TMap<TObjectPtr<UInstancedStaticMeshComponent>, TObjectPtr<USAS_ResourceTypeData>> ComponentTypeMap;

protected:

	//TEMPORARY
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TObjectPtr<USAS_ResourceTypeData> DefaultTypeData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource")
	TMap<TObjectPtr<UInstancedStaticMeshComponent>, FGuid> ClusterGuids;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource")
	TMap<FName, TObjectPtr<USAS_ResourceTypeData>> TagToResourceType;

private:


	TMap<TObjectPtr<const UInstancedStaticMeshComponent>, TSet<int32>> DisabledInstancesByComponent;







};
