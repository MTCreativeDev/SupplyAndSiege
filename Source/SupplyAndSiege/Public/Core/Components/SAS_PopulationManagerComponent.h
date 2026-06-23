

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_PopulationManagerComponent.generated.h"

class ASAS_SelectablePawn;
class ASAS_SelectableBuilding;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_PopulationManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_PopulationManagerComponent();

	void RegisterHousing(AActor* HousingActor);
	void UnregisterHousing(AActor* HousingActor);

	void RegisterVillager(AActor* VillagerActor);
	void UnregisterVillager(AActor* VillagerActor);

	UFUNCTION(BlueprintCallable, Category = "Housing")
	int32 GetCurrentMaxPopulation();

	UFUNCTION(BlueprintCallable, Category = "Housing")
	int32 GetCurrentPopulationCount();


protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TrySpawnVillager();


private:
	int32 GetAvailableHousingSpace();




public:	



protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> CurrentPopulation;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> CurrentHouses;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Population")
	int32 PopulationPerHouse = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Population")
	TSubclassOf<ASAS_SelectablePawn> VillagerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Population")
	TSubclassOf<ASAS_SelectableBuilding> StorageDepotClass;

	TArray<FVector> VillagerSpawnLocations;

	UPROPERTY()
	float SpawnCheckInterval = 5.f;

	UPROPERTY()
	float SpawnChance = .25;
		
};
