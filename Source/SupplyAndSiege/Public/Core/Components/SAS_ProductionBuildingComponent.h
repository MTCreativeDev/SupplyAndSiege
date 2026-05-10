#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/DataAssets/SAS_GameDataAsset.h"
#include "Misc/DataAssets/SAS_RecipeData.h"
#include "SAS_ProductionBuildingComponent.generated.h"

USTRUCT(BlueprintType)
struct FProductionRequirements
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Production")
	TMap<USAS_GameDataAsset*, int32> RequiredCounts;
};

// Declare the delegate for the requirements updated event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequirementsUpdatedDelegate, const FProductionRequirements&, Requirements);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_ProductionBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USAS_ProductionBuildingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Queue")
	TArray<TObjectPtr<USAS_GameDataAsset>> ProductionQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipes")
	TArray<TObjectPtr<USAS_RecipeData>> Recipes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<TObjectPtr<USAS_GameDataAsset>, int32> Inventory;

	TMap<TObjectPtr<USAS_GameDataAsset>, TObjectPtr<USAS_RecipeData>> RecipesMap;

	TMap<USAS_GameDataAsset*, int32> GetTotalRequiredCounts();

	TMap<USAS_GameDataAsset*, int32> GetRequiredCountsForProduct(USAS_GameDataAsset* Product);

	bool HaveRequirementsForProduct(USAS_GameDataAsset* Product);

	UFUNCTION(BlueprintCallable, Category = "Production")
	bool AddProductToQueue(USAS_GameDataAsset* Product);

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	void BuildRecipesMap();  // Helper to populate RecipesMap from Recipes
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Event dispatcher for when requirements are updated (assignable in Blueprints)
	UPROPERTY(BlueprintAssignable, Category = "Production")
	FOnRequirementsUpdatedDelegate OnRequirementsUpdated;

};