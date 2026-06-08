


#include "Core/Components/SAS_UnitSightComponent.h"
#include "Components/SphereComponent.h"
#include "Core/CustomCollision.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_VisibilityManagerComponent.h"
#include "GameFramework/GameStateBase.h"


USAS_UnitSightComponent::USAS_UnitSightComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionProfileName(SAS_CollisionProfiles::UnitSight);
	SetCanEverAffectNavigation(false);
	SetGenerateOverlapEvents(true);

}


void USAS_UnitSightComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();

	if (!Owner) return;

	OwnerUnitInfo = Owner->FindComponentByClass<USAS_UnitInformationComponent>();

	SetSphereRadius(SightRadius);

	OnComponentBeginOverlap.AddDynamic(this, &USAS_UnitSightComponent::HandleSightBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USAS_UnitSightComponent::HandleSightEndOverlap);

	UWorld* World = GetWorld();
	AGameStateBase* GS = World->GetGameState();
	VisibilityManagerComponent = GS->FindComponentByClass<USAS_VisibilityManagerComponent>();

	UpdateOverlaps();

	ensureMsgf(
		VisibilityManagerComponent,
		TEXT("%s is missing VisibilityManagerComponent"),
		*GetNameSafe(GetOwner())
	);

	
	
}

void USAS_UnitSightComponent::HandleSightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	AActor* Owner = GetOwner();
	if (!Owner || !VisibilityManagerComponent) return;
	
	if (!IsValidEnemy(OtherActor)) return;
	if (VisibleEnemies.Contains(OtherActor)) return;

	VisibleEnemies.Add(OtherActor);

	if (OwnerUnitInfo && OwnerUnitInfo->GetTeam() == ESAS_Team::Team1)
	{
		VisibilityManagerComponent->RegisterSeenEnemy(OtherActor, Owner);
	}

	OnEnemyDetected.Broadcast(OtherActor);

}

void USAS_UnitSightComponent::HandleSightEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	AActor* Owner = GetOwner();
	if (!Owner || !VisibilityManagerComponent) return;

	if (!VisibleEnemies.Contains(OtherActor)) return;

	VisibleEnemies.Remove(OtherActor);

	if (OwnerUnitInfo && OwnerUnitInfo->GetTeam() == ESAS_Team::Team1)
	{
		VisibilityManagerComponent->RegisterLostEnemy(OtherActor, Owner);
	}

	OnEnemyLost.Broadcast(OtherActor);

}

bool USAS_UnitSightComponent::IsValidEnemy(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == GetOwner()) return false;
	if (!OwnerUnitInfo) return false;

	const USAS_UnitInformationComponent* OtherUnitInfo = OtherActor->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!OtherUnitInfo) return false;

	return OtherUnitInfo->GetTeam() != OwnerUnitInfo->GetTeam();
}




