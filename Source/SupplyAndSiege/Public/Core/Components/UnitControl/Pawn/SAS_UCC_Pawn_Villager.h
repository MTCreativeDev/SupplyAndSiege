

#pragma once

#include "CoreMinimal.h"
#include "Core/Components/UnitControl/SAS_UCC_Pawn.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "SAS_UCC_Pawn_Villager.generated.h"

class USAS_WorkerControlComponent;
class USAS_ResourceTypeData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_UCC_Pawn_Villager : public USAS_UCC_Pawn
{
	GENERATED_BODY()

public:
	virtual bool GroundSelectIsMove() const override;
	virtual void HandleGroundRightClick(const FVector& WorldLocation) override;

	virtual bool ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const override;
	virtual void HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	TObjectPtr<USAS_WorkerControlComponent> WorkerControl = nullptr;
	
};
