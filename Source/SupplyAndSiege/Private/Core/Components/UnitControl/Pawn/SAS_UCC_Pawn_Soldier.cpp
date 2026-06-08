


#include "Core/Components/UnitControl/Pawn/SAS_UCC_Pawn_Soldier.h"
#include "Core/SAS_GameplayTagContainer.h"

bool USAS_UCC_Pawn_Soldier::GroundSelectIsMove() const
{
	return true;
}

void USAS_UCC_Pawn_Soldier::HandleGroundRightClick(const FVector& WorldLocation)
{
	BeginMove_Internal(WorldLocation);
}

bool USAS_UCC_Pawn_Soldier::ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const
{
	return true;
}

void USAS_UCC_Pawn_Soldier::HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	BeginMove_Internal(ResourceLocation);
}

bool USAS_UCC_Pawn_Soldier::CanReceiveAttackOrder() const
{
	return true;
}

void USAS_UCC_Pawn_Soldier::HandleAttackUnitOrder(USAS_UnitInformationComponent* TargetUnitInformationComponent, const FVector& ClickLocation)
{
	if (!TargetUnitInformationComponent) return;

	CurrentAttackTarget = TargetUnitInformationComponent;
	CurrentPawnControlState = ESAS_PawnControlState::Attacking;
	SendStateTreeEvent(SASGameplayTags::StateTree_Combat_AttackTarget);
}

void USAS_UCC_Pawn_Soldier::BeginPlay()
{
}
