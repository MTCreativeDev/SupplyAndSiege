

#pragma once

#include "CoreMinimal.h"
#include "SAS_SourceTargetPair.generated.h"

/**
 * 
 */
USTRUCT()
struct FSAS_SourceTargetPair
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Source = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> Target = nullptr;

	bool operator==(const FSAS_SourceTargetPair& Other) const
	{
		return Source == Other.Source && Target == Other.Target;
	}

};

FORCEINLINE uint32 GetTypeHash(const FSAS_SourceTargetPair& Pair)
{
	return HashCombine(
		GetTypeHash(Pair.Source),
		GetTypeHash(Pair.Target)
	);
}