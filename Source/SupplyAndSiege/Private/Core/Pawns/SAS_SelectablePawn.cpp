// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Pawns/SAS_SelectablePawn.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Core/Components/SAS_UnitControlComponent.h"
#include "Core/Components/SAS_UnitSightComponent.h"
#include "Components/SkeletalMeshComponent.h"


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

	UnitSightComponent = CreateDefaultSubobject<USAS_UnitSightComponent>(TEXT("UnitSightComponent"));
	UnitSightComponent->SetupAttachment(RootComponent);

}

UPawnMovementComponent* ASAS_SelectablePawn::GetMovementComponent() const
{
	return MovementComponent;
}


void ASAS_SelectablePawn::SetVisibleToPlayer_Implementation(bool bVisible)
{
	bVisibleToPlayer = bVisible;
	SetActorHiddenInGame(!bVisible);


	//Placeholder until we have a skm. Mainly trying to future proof and expects only 1 skm.
	if (USkeletalMeshComponent* SkelMesh = FindComponentByClass<USkeletalMeshComponent>())
	{
		SkelMesh->SetVisibility(bVisible, true);
		SkelMesh->SetComponentTickEnabled(bVisible);
		SkelMesh->bPauseAnims = !bVisible;
	}
}

bool ASAS_SelectablePawn::IsVisibleToPlayer_Implementation() const
{
	return bVisibleToPlayer;
}

void ASAS_SelectablePawn::BeginPlay()
{
	Super::BeginPlay();

	UnitInformationComponent->SetTeam(AssignTeamOnSpawn);

	ApplyInitialTeamVisibility();

	USAS_UnitControlComponent* ControlComp = FindComponentByClass<USAS_UnitControlComponent>();
	if (!ControlComp)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				FString::Printf(TEXT("%s is missing UnitControlComponent"), *GetNameSafe(GetOwner()))
			);
		}

		ensureMsgf(false, TEXT("%s is missing UnitControlComponent"), *GetNameSafe(GetOwner()));
	}
	
}

void ASAS_SelectablePawn::DestroySelf()
{
	UnitInformationComponent->RemoveUnitFromGame();
	Destroy();
}

void ASAS_SelectablePawn::ApplyInitialTeamVisibility()
{
	const ESAS_Team Team = UnitInformationComponent->GetTeam();

	const bool bIsPlayerTeam = Team == ESAS_Team::Team1;

	ISAS_PlayerVisibilityInterface::Execute_SetVisibleToPlayer(this, bIsPlayerTeam);
}