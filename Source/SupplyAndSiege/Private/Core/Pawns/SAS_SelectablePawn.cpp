// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Pawns/SAS_SelectablePawn.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


ASAS_SelectablePawn::ASAS_SelectablePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	UnitInformationComponent = CreateDefaultSubobject<USAS_UnitInformationComponent>(TEXT("UnitInformationComponent"));

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Capsule->SetCollisionObjectType(ECC_Pawn);
	//TODO: This is temporary. Pawns won't really be blocking everything in the finished game. Need to fix later.
	Capsule->SetCollisionResponseToAllChannels(ECR_Block);
	Capsule->SetSimulatePhysics(false);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
	MovementComponent->UpdatedComponent = RootComponent;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;



}

UPawnMovementComponent* ASAS_SelectablePawn::GetMovementComponent() const
{
	return MovementComponent;
}

void ASAS_SelectablePawn::BeginPlay()
{
	Super::BeginPlay();

	UnitInformationComponent->SetTeam(AssignTeamOnSpawn);
	
}

void ASAS_SelectablePawn::DestroySelf()
{
	UnitInformationComponent->RemoveUnitFromGame();
	Destroy();
}

