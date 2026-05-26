

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_VisibilityManagerComponent.generated.h"

USTRUCT()
struct FSAS_VisibilityRecord
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> SeeingActors;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_VisibilityManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USAS_VisibilityManagerComponent();

	void RegisterSeenEnemy(AActor* SeenEnemy, AActor* SeeingAlly);
	void RegisterLostEnemy(AActor* SeenEnemy, AActor* SeeingAlly);

protected:

	virtual void BeginPlay() override;

private:

	void RefreshEnemyVisibility(AActor* SeenEnemy, ESAS_Team ViewingTeam);

	TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord>* GetVisibleEnemyMapForTeam(ESAS_Team Team);

public:	

private:

	TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord> Team1VisibleEnemyUnits;
	TMap<TWeakObjectPtr<AActor>, FSAS_VisibilityRecord> Team2VisibleEnemyUnits;
		
};
