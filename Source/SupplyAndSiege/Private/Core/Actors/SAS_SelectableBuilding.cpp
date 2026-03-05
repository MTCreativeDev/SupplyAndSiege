
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ASAS_SelectableBuilding::ASAS_SelectableBuilding()
{
	PrimaryActorTick.bCanEverTick = false;

	UnitInformationComponent = CreateDefaultSubobject<USAS_UnitInformationComponent>(TEXT("UnitInformationComponent"));

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));

	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//TODO: Need a custom collision channel for this I think.
	Box->SetCollisionObjectType(ECC_Pawn);
	//TODO: This is temporary. Pawns won't really be blocking everything in the finished game. Need to fix later.
	Box->SetCollisionResponseToAllChannels(ECR_Block);
	Box->SetSimulatePhysics(false);
	Box->SetupAttachment(SceneRoot);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Box);



}


void ASAS_SelectableBuilding::BeginPlay()
{
	Super::BeginPlay();

	UnitInformationComponent->SetTeam(AssignTeamOnSpawn);
	
}

void ASAS_SelectableBuilding::DestroySelf()
{
	UnitInformationComponent->RemoveUnitFromGame();
	Destroy();
}

float ASAS_SelectableBuilding::GetDefaultHalfHeight()
{
	USceneComponent* DefaultRoot = GetClass()->GetDefaultObject<ASAS_SelectableBuilding>()->RootComponent;
	if (DefaultRoot)
	{
		float Radius, HalfHeight;
		DefaultRoot->UpdateBounds(); // Since it's the default object, it wouldn't have been registered to ever do this.
		DefaultRoot->CalcBoundingCylinder(Radius, HalfHeight);
		return HalfHeight;
	}
	else
	{
		// This will probably fail to return anything useful, since default objects won't have registered components,
		// but at least it will spit out a warning if so.
		return GetClass()->GetDefaultObject<APawn>()->GetSimpleCollisionHalfHeight();
	}
}

