

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SAS_ShowcaseData.generated.h"

class UStaticMesh;
class USkeletalMesh;

UENUM(BlueprintType)
enum class EMeshType : uint8
{
	StaticMesh	UMETA(DisplayName = "Static Mesh"),
	SkeletalMesh	UMETA(DisplayName = "Skeletal Mesh")
};


UCLASS(BlueprintType)
class SUPPLYANDSIEGE_API USAS_ShowcaseData : public UDataAsset
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Showcase")
	FName DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Showcase")
	EMeshType MeshType = EMeshType::SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Showcase")
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Showcase")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Showcase")
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Showcase")
	USkeletalMesh* SkeletalMesh;

};
