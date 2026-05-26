


#include "Core/Components/SAS_VisibilityManagerComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Interfaces/SAS_PlayerVisibilityInterface.h"

USAS_VisibilityManagerComponent::USAS_VisibilityManagerComponent()
{

	PrimaryComponentTick.bCanEverTick = false;


}

void USAS_VisibilityManagerComponent::RegisterSeenEnemy(AActor* SeenEnemy, AActor* SeeingAlly)
{
	if (!IsValid(SeenEnemy) || !IsValid(SeeingAlly)) return;

	const USAS_UnitInformationComponent* SeeingAllyUnitInfo = SeeingAlly->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!SeeingAllyUnitInfo) return;

	const ESAS_Team SeeingTeam = SeeingAllyUnitInfo->GetTeam();

	TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord>* VisibleEnemyMap = GetVisibleEnemyMapForTeam(SeeingTeam);
	if (!VisibleEnemyMap) return;

	FSAS_VisibilityRecord& Record = VisibleEnemyMap->FindOrAdd(SeenEnemy);

	const bool bWasHidden = Record.SeeingActors.Num() == 0;

	Record.SeeingActors.Add(SeeingAlly);

	if (bWasHidden)
	{
		RefreshEnemyVisibility(SeenEnemy, SeeingTeam);
	}
}

void USAS_VisibilityManagerComponent::RegisterLostEnemy(AActor* SeenEnemy, AActor* SeeingAlly)
{
	if (!IsValid(SeenEnemy) || !IsValid(SeeingAlly)) return;

	const USAS_UnitInformationComponent* SeeingAllyUnitInfo = SeeingAlly->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!SeeingAllyUnitInfo) return;

	const ESAS_Team SeeingTeam = SeeingAllyUnitInfo->GetTeam();

	TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord>* VisibleEnemyMap = GetVisibleEnemyMapForTeam(SeeingTeam);
	if (!VisibleEnemyMap) return;

	FSAS_VisibilityRecord* Record = VisibleEnemyMap->Find(SeenEnemy);
	if (!Record) return;

	Record->SeeingActors.Remove(SeeingAlly);

	for (auto SeeingActorIterator = Record->SeeingActors.CreateIterator(); SeeingActorIterator; ++SeeingActorIterator)
	{
		if (!SeeingActorIterator->IsValid())
		{
			SeeingActorIterator.RemoveCurrent();
		}
	}

	if (Record->SeeingActors.Num() == 0)
	{
		RefreshEnemyVisibility(SeenEnemy, SeeingTeam);
		VisibleEnemyMap->Remove(SeenEnemy);
	}

}

void USAS_VisibilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();


}

void USAS_VisibilityManagerComponent::RefreshEnemyVisibility(AActor* SeenEnemy, ESAS_Team ViewingTeam)
{
	if (!IsValid(SeenEnemy)) return;

	TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord>* VisibleEnemyMap = GetVisibleEnemyMapForTeam(ViewingTeam);
	if (!VisibleEnemyMap) return;

	const FSAS_VisibilityRecord* Record = VisibleEnemyMap->Find(SeenEnemy);

	const bool bShouldBeVisible = Record && Record->SeeingActors.Num() > 0;

	if (SeenEnemy->Implements<USAS_PlayerVisibilityInterface>())
	{
		ISAS_PlayerVisibilityInterface::Execute_SetVisibleToPlayer(SeenEnemy, bShouldBeVisible);
	}
}

TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord>* USAS_VisibilityManagerComponent::GetVisibleEnemyMapForTeam(ESAS_Team Team)
{
	switch (Team)
	{
	case ESAS_Team::Team1:
		return &Team1VisibleEnemyUnits;

	case ESAS_Team::Team2:
		return &Team2VisibleEnemyUnits;

	default:
		return nullptr;
	}
}
