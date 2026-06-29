


#include "Core/Components/SAS_WinLossManagerComponent.h"
#include "Core/Actors/SAS_BuildingLayout.h"
#include "Misc/Structs/SAS_BuildingWinRequirement.h"

USAS_WinLossManagerComponent::USAS_WinLossManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}



void USAS_WinLossManagerComponent::NotifyBuildingCompleted(ASAS_BuildingLayout* CompletedBuilding)
{
	if (bGameEnded || !IsValid(CompletedBuilding)) return;

	for (FSAS_BuildingWinRequirement& Requirement : BuildingRequirements)
	{
		if (!Requirement.BuildingClass) continue;

		if (CompletedBuilding->IsA(Requirement.BuildingClass))
		{
			Requirement.CurrentAmount++;
			OnBuildingWinRequirementsChanged.Broadcast(BuildingRequirements);
			CheckWinCondition();
			return;
		}
	}
}

void USAS_WinLossManagerComponent::NotifyAllVillagersDead()
{
}

void USAS_WinLossManagerComponent::CheckWinCondition()
{
	if (bGameEnded) return;

	for (const FSAS_BuildingWinRequirement& Requirement : BuildingRequirements)
	{
		if (Requirement.CurrentAmount < Requirement.RequiredAmount)
		{
			return;
		}
	}
		
	bGameEnded = true;
	HandleGameWon();
}
