#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Misc/DataAssets/SAS_DamageTypeData.h"
#include "SAS_AttackDefinitionData.generated.h"


UENUM(BlueprintType)
enum class ESAS_AttackType : uint8
{
	Melee	UMETA(DisplayName = "Melee"),
	Ranged	UMETA(DisplayName = "Ranged"),
	Magic	UMETA(DisplayName = "Magic")
};

UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_AttackDefinitionData: public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	ESAS_AttackType AttackType = ESAS_AttackType::Melee;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRange = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float Cooldown = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	USAS_DamageTypeData* DamageType;

};
