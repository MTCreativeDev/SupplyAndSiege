


#include "Core/Components/SAS_PopulationManagerComponent.h"
#include "Core/Pawns/SAS_SelectablePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Actors/SAS_SelectableBuilding.h"


USAS_PopulationManagerComponent::USAS_PopulationManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = SpawnCheckInterval;
}

void USAS_PopulationManagerComponent::RegisterHousing(AActor* HousingActor)
{
	if (!IsValid(HousingActor)) return;

	CurrentHouses.AddUnique(HousingActor);

	NotifyPopulationChange();
}

void USAS_PopulationManagerComponent::UnregisterHousing(AActor* HousingActor)
{
	if (!HousingActor) return;

	CurrentHouses.RemoveAll([HousingActor](const TWeakObjectPtr<AActor>& ExistingHouse)
		{
			return !ExistingHouse.IsValid() || ExistingHouse.Get() == HousingActor;
		});

	NotifyPopulationChange();
}

void USAS_PopulationManagerComponent::RegisterVillager(AActor* VillagerActor)
{
	if (!IsValid(VillagerActor)) return;

	CurrentPopulation.AddUnique(VillagerActor);

	NotifyPopulationChange();
}

void USAS_PopulationManagerComponent::UnregisterVillager(AActor* VillagerActor)
{
	if (!VillagerActor) return;

	CurrentPopulation.RemoveAll([VillagerActor](const TWeakObjectPtr<AActor>& ExistingVillager)
		{
			return !ExistingVillager.IsValid() || ExistingVillager.Get() == VillagerActor;
		});

	NotifyPopulationChange();
}

int32 USAS_PopulationManagerComponent::GetCurrentMaxPopulation()
{
	CurrentHouses.RemoveAll([](const TWeakObjectPtr<AActor>& House)
		{
			return !House.IsValid();
		});
		

	return CurrentHouses.Num() * PopulationPerHouse;
}

int32 USAS_PopulationManagerComponent::GetCurrentPopulationCount()
{
	CurrentPopulation.RemoveAll([](const TWeakObjectPtr<AActor>& Villager)
		{
			return !Villager.IsValid();
		});


	return CurrentPopulation.Num();
}

void USAS_PopulationManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!StorageDepotClass) return;

	TArray<AActor*> FoundDepots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StorageDepotClass, FoundDepots);

	if (FoundDepots.Num() < 1) return;

	ASAS_SelectableBuilding* StorageDepot = Cast<ASAS_SelectableBuilding>(FoundDepots[0]);
	if (!IsValid(StorageDepot)) return;

	VillagerSpawnLocations = StorageDepot->GetMoveToLocations();

}

void USAS_PopulationManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TrySpawnVillager();
}

void USAS_PopulationManagerComponent::TrySpawnVillager()
{
	int32 AvailableHousingSpace = GetAvailableHousingSpace();

	if (AvailableHousingSpace < 1) return;
		
	if (!VillagerClass) return;

	if (FMath::FRand() > SpawnChance) return;
		
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Try Spawn"));
	if (VillagerSpawnLocations.Num() < 1) return;

	int32 VillagersToSpawn = 1;

	//Hardcoding spawning multiple villagers as a 10% chance (if there is space)

	if (AvailableHousingSpace >= 2)
	{
		if (FMath::FRand() <= .1)
		{
			VillagersToSpawn = 2;
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Villagers To Spawn: %d"), VillagersToSpawn));

	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < VillagersToSpawn; ++i)
	{
		const int32 RandomIndex = FMath::RandRange(0, VillagerSpawnLocations.Num() - 1);
		const FVector SpawnLocation = VillagerSpawnLocations[RandomIndex];

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);

		ASAS_SelectablePawn* NewVillager = World->SpawnActorDeferred<ASAS_SelectablePawn>(
			VillagerClass,
			SpawnTransform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		if (!IsValid(NewVillager)) return;

		NewVillager->AssignTeamOnSpawn = ESAS_Team::Team1;

		UGameplayStatics::FinishSpawningActor(NewVillager, SpawnTransform);
	}
}

int32 USAS_PopulationManagerComponent::GetAvailableHousingSpace()
{
	return GetCurrentMaxPopulation() - GetCurrentPopulationCount();
}

void USAS_PopulationManagerComponent::NotifyPopulationChange()
{
	OnPopulationChange.Broadcast(GetCurrentPopulationCount(),GetCurrentMaxPopulation());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Added"));
}

