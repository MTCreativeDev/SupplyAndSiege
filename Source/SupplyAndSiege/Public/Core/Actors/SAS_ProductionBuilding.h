#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/DataAssets/SAS_GameDataAsset.h"
#include "Misc/DataAssets/SAS_RecipeData.h"
#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Core/Actors/SAS_ResourceGenSelectableBuilding.h"
#include "SAS_ProductionBuilding.generated.h"


USTRUCT(BlueprintType)
struct FProductionRequirements
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Production")
	TMap<TObjectPtr<USAS_GameDataAsset>, int32> RequiredCounts;
};

UCLASS()
class SUPPLYANDSIEGE_API ASAS_ProductionBuilding : public ASAS_ResourceGenSelectableBuilding
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ASAS_ProductionBuilding();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Queue")
	TArray<TObjectPtr<USAS_GameDataAsset>> ProductionQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipes")
	TArray<TObjectPtr<USAS_RecipeData>> Recipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<TObjectPtr<USAS_GameDataAsset>, int32> Inventory;

	TArray<USAS_LogisticsMasterJob*> ActiveLogisticsJobs;

	TMap<TObjectPtr<USAS_GameDataAsset>, TObjectPtr<USAS_RecipeData>> RecipesMap;

	TMap<TObjectPtr<USAS_GameDataAsset>, int32> GetTotalRequiredCounts();

	TMap<TObjectPtr<USAS_GameDataAsset>, int32> GetRequiredCountsForProduct(USAS_GameDataAsset* Product);

	bool HaveRequirementsForProduct(USAS_GameDataAsset* Product);

	UFUNCTION(BlueprintCallable, Category = "Production")
	bool AddProductToQueue(USAS_GameDataAsset* Product);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	void BuildRecipesMap();  // Helper to populate RecipesMap from Recipes


};