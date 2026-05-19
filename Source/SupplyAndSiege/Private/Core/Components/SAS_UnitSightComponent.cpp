


#include "Core/Components/SAS_UnitSightComponent.h"
#include "Components/SphereComponent.h"
#include "Core/CustomCollision.h"
#include "Core/Components/SAS_UnitInformationComponent.h"


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
	
}

void USAS_UnitSightComponent::HandleSightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweek, const FHitResult& SweepResult)
{
	if (!IsValidEnemy(OtherActor)) return;

	VisibleEnemies.AddUnique(OtherActor);
	OnEnemyDetected.Broadcast(OtherActor);
}

void USAS_UnitSightComponent::HandleSightEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	const int32 RemovedCount = VisibleEnemies.Remove(OtherActor);

	if (RemovedCount > 0)
	{
		OnEnemyLost.Broadcast(OtherActor);
	}
}

bool USAS_UnitSightComponent::IsValidEnemy(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == GetOwner()) return false;
	if (!OwnerUnitInfo) return false;

	const USAS_UnitInformationComponent* OtherUnitInfo = OtherActor->FindComponentByClass<USAS_UnitInformationComponent>();
	if (!OtherUnitInfo) return false;

	return OtherUnitInfo->GetTeam() != OwnerUnitInfo->GetTeam();
}


