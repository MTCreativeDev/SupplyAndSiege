#include "Core/Components/SAS_ProductionBuidlingComponent.h"

// Sets default values for this component's properties
USAS_ProductionBuidlingComponent::USAS_ProductionBuidlingComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void USAS_ProductionBuidlingComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...

}


// Called every frame
void USAS_ProductionBuidlingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

TMap<USAS_GameDataAsset*, int32> USAS_ProductionBuidlingComponent::GetTotalRequiredCounts()
{
    TMap<USAS_GameDataAsset*, int32> TotalRequiredCounts;
    for (TObjectPtr<USAS_GameDataAsset> Product : ProductionQueue)
    {
        if (!Product)
            continue;

        TMap<USAS_GameDataAsset*, int32> ProductRequirements = GetRequiredCountsForProduct(Product.Get());
        for (auto& Pair : ProductRequirements)
        {
            TotalRequiredCounts.FindOrAdd(Pair.Key) += Pair.Value;
        }

    }

    // Subtract inventory counts
    for (auto& Pair : TotalRequiredCounts)
    {
        int32 Available = Inventory.FindRef(Pair.Key);
        Pair.Value -= Available;
        if (Pair.Value <= 0)
        {
            TotalRequiredCounts.Remove(Pair.Key);
        }
    }

    return TotalRequiredCounts;
}

TMap<USAS_GameDataAsset*, int32> USAS_ProductionBuidlingComponent::GetRequiredCountsForProduct(USAS_GameDataAsset* Product)
{
    TMap<USAS_GameDataAsset*, int32> RequiredCounts;
    TSet<USAS_GameDataAsset*> Visited;

    // Helper lambda to recursively collect requirements
    auto CollectRequirements = [&](auto&& Self, USAS_GameDataAsset* Asset) -> void
        {
            if (Visited.Contains(Asset) || !Asset)
            {
                return;
            }
            Visited.Add(Asset);

            if (!Recipes.Contains(Asset))
            {
                Visited.Remove(Asset);
                return;
            }

            const USAS_RecipeData* Recipe = Recipes[Asset];
            if (!Recipe)
            {
                Visited.Remove(Asset);
                return;
            }

            for (TObjectPtr<USAS_GameDataAsset> Req : Recipe->Requirements)
            {
                if (!Req)
                    continue;
                RequiredCounts.FindOrAdd(Req.Get())++;
                Self(Self, Req.Get());  // Recursive call

            }


            Visited.Remove(Asset);
        };

    CollectRequirements(CollectRequirements, Product);

    return RequiredCounts;
}

bool USAS_ProductionBuidlingComponent::HaveRequirementsForProduct(USAS_GameDataAsset* Product)
{
    TMap<USAS_GameDataAsset*, int32> RequiredCounts = GetRequiredCountsForProduct(Product);
    for (auto& Pair : RequiredCounts)
    {
        int32 Available = Inventory.FindRef(Pair.Key);
        if (Available < Pair.Value)
        {
            return false;
        }
    }
    return true;
}

// Returns if the product was successfully added to the queue (i.e. it exists and has a recipe)
bool USAS_ProductionBuidlingComponent::AddProductToQueue(USAS_GameDataAsset* Product)
{
    if (!Product || !Recipes.Contains(Product))
        return false;

    ProductionQueue.Add(Product);
    TMap<USAS_GameDataAsset*, int32> RequiredCounts = GetTotalRequiredCounts();

    if (RequiredCounts.Num() > 0)
    {
		FProductionRequirements requirements = { RequiredCounts };

        // Broadcast the event with the required counts
        OnRequirementsUpdated.Broadcast(requirements);

        // Log the required counts for debugging
        UE_LOG(LogTemp, Warning, TEXT("Total Required Counts after adding product to queue:"));
        for (auto& Pair : RequiredCounts)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: %d"), *Pair.Key->GetName(), Pair.Value);
        }
    }


    return true;
}