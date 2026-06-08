

#pragma once

#include "CoreMinimal.h"
#include "Core/Components/UnitControl/SAS_UCC_Pawn.h"
#include "SAS_UCC_Pawn_Soldier.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_UCC_Pawn_Soldier : public USAS_UCC_Pawn
{
	GENERATED_BODY()
	
public:
	virtual bool GroundSelectIsMove() const override;
	virtual void HandleGroundRightClick(const FVector& WorldLocation) override;

	virtual bool ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const override;
	virtual void HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation) override;

	virtual bool CanReceiveAttackOrder() const override;
	virtual void HandleAttackUnitOrder(USAS_UnitInformationComponent* TargetUnitInformationComponent, const FVector& ClickLocation) override;
protected:
	virtual void BeginPlay() override;

protected:

private:



};
