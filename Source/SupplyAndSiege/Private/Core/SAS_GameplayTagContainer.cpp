


#include "Core/SAS_GameplayTagContainer.h"

namespace SASGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(StateTree_Pawn_MoveOrder, "StateTree.Pawn.MoveOrder");
	UE_DEFINE_GAMEPLAY_TAG(StateTree_Worker_HarvestOrder, "StateTree.Worker.HarvestOrder");
	UE_DEFINE_GAMEPLAY_TAG(StateTree_Worker_TransitionToLogistics, "StateTree.Worker.TransitionToLogistics");
	UE_DEFINE_GAMEPLAY_TAG(StateTree_Worker_Logistics_Unassigned, "StateTree.Worker.Logistics.Unassigned");
	UE_DEFINE_GAMEPLAY_TAG(StateTree_Worker_Logistics_DeliverItem_MoveToSource, "StateTree.Worker.Logistics.DeliverItem.MoveToSource");
	UE_DEFINE_GAMEPLAY_TAG(StateTree_Worker_Logistics_DeliverItem_MoveToTarget, "StateTree.Worker.Logistics.DeliverItem.MoveToTarget");
}