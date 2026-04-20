


#include "Core/Components/UnitControl/SAS_UCC_Pawn.h"
#include "AIController.h"
#include "GameFramework/GameStateBase.h"
#include "Core/SAS_GameplayTagContainer.h"
#include "GameFramework/Pawn.h"
#include "Components/StateTreeComponent.h"


bool USAS_UCC_Pawn::GroundSelectIsMove() const
{
	return true;
}

void USAS_UCC_Pawn::HandleGroundRightClick(const FVector& WorldLocation)
{
	BeginMove_Internal(WorldLocation);
}

bool USAS_UCC_Pawn::ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const
{
	return true;
}

void USAS_UCC_Pawn::BeginMove_Internal(const FVector& WorldLocation)
{
	CurrentMoveTarget = WorldLocation;
	CurrentPawnControlState = ESAS_PawnControlState::Moving;
	SendStateTreeEvent(SASGameplayTags::StateTree_Pawn_MoveOrder);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Move Received"));
	}

}

void USAS_UCC_Pawn::SendStateTreeEvent(const FGameplayTag& EventTag)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn) return;

	AAIController* AI = Cast<AAIController>(Pawn->GetController());
	if (!AI) return;

	UStateTreeComponent* ST = AI->FindComponentByClass<UStateTreeComponent>();
	if (!ST) return;

	ST->SendStateTreeEvent(FStateTreeEvent(EventTag));
}
