// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAS_ResourceKey.generated.h"


USTRUCT(BlueprintType)
struct SUPPLYANDSIEGE_API FSAS_ResourceKey
{
	GENERATED_BODY()

public:

	//Default no param constructor
	FSAS_ResourceKey() = default;

	//Constructor used to construct a proper key with the actual GUID and InstanceIndex
	FSAS_ResourceKey(const FGuid& InClusterGuid, const int32 InInstanceIndex)
		: ClusterGuid(InClusterGuid),
		InstanceIndex(InInstanceIndex)
	{ }

	bool IsValid() const
	{
		return ClusterGuid.IsValid() && InstanceIndex != INDEX_NONE;
	}


	FString ToString() const
	{
		return FString::Printf(TEXT(" % s: % d"), *ClusterGuid.ToString(EGuidFormats::DigitsWithHyphensLower), InstanceIndex);
	}


	//These functions handle standard evaluator functions (Does this ResourceKey equal this other ResourceKey).
	friend bool operator==(const FSAS_ResourceKey& A, const FSAS_ResourceKey& B)
	{
		return A.ClusterGuid == B.ClusterGuid && A.InstanceIndex == B.InstanceIndex;
	}

	friend bool operator!=(const FSAS_ResourceKey& A, const FSAS_ResourceKey& B)
	{
		return  !(A == B);
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FGuid ClusterGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	int32 InstanceIndex = INDEX_NONE;

};

//Not sure if we will need this, but just in case we need to use this as a key in a Map or Set
FORCEINLINE uint32 GetTypeHash(const FSAS_ResourceKey& Key)
{
	return HashCombine(GetTypeHash(Key.ClusterGuid), GetTypeHash(Key.InstanceIndex));
}