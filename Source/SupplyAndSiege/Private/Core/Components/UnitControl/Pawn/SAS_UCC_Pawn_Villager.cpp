


#include "Core/Components/UnitControl/Pawn/SAS_UCC_Pawn_Villager.h"
#include "Core/Components/SAS_WorkerControlComponent.h"
#include "Misc/DataAssets/SAS_ResourceTypeData.h"

bool USAS_UCC_Pawn_Villager::GroundSelectIsMove() const
{
	return true;
}

void USAS_UCC_Pawn_Villager::HandleGroundRightClick(const FVector& WorldLocation)
{
	if (!WorkerControl) return;

	const ESAS_WorkerRequestResult Result = WorkerControl->RequestManualMove(WorldLocation);
	if (Result == ESAS_WorkerRequestResult::Rejected) return;

	BeginMove_Internal(WorldLocation);
}

bool USAS_UCC_Pawn_Villager::ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const
{
	return ResourceType == nullptr;
}

void USAS_UCC_Pawn_Villager::HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	if (!WorkerControl || !ResourceType) return;
	WorkerControl->RequestManualHarvest(ResourceType, ResourceKey, ResourceLocation);
}

void USAS_UCC_Pawn_Villager::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		WorkerControl = Owner->FindComponentByClass<USAS_WorkerControlComponent>();
	}
}
