#include "Core/Actors/SAS_ProductionBuilding.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Core/SAS_GameState.h"


// Sets default values for this component's properties
ASAS_ProductionBuilding::ASAS_ProductionBuilding()
{
   
}


// Called when the game starts
void ASAS_ProductionBuilding::BeginPlay()
{
    Super::BeginPlay();
    BuildRecipesMap();
    // ...

}


TMap<TObjectPtr<USAS_GameDataAsset>, int32> ASAS_ProductionBuilding::GetTotalRequiredCounts()
{
    TMap<TObjectPtr<USAS_GameDataAsset>, int32> TotalRequiredCounts;
    for (TObjectPtr<USAS_GameDataAsset> Product : ProductionQueue)
    {
        if (!Product)
            continue;

        TMap<TObjectPtr<USAS_GameDataAsset>, int32> ProductRequirements = GetRequiredCountsForProduct(Product.Get());
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

TMap<TObjectPtr<USAS_GameDataAsset>, int32> ASAS_ProductionBuilding::GetRequiredCountsForProduct(USAS_GameDataAsset* Product)
{
    TMap<TObjectPtr<USAS_GameDataAsset>, int32> RequiredCounts;
    TSet<USAS_GameDataAsset*> Visited;

    // Helper lambda to recursively collect requirements
    auto CollectRequirements = [&](auto&& Self, USAS_GameDataAsset* Asset) -> void
        {
            if (Visited.Contains(Asset) || !Asset)
            {
                return;
            }
            Visited.Add(Asset);

            if (!RecipesMap.Contains(Asset))
            {
                Visited.Remove(Asset);
                return;
            }

            const USAS_RecipeData* Recipe = RecipesMap[Asset];
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

bool ASAS_ProductionBuilding::HaveRequirementsForProduct(USAS_GameDataAsset* Product)
{
    TMap<TObjectPtr<USAS_GameDataAsset>, int32> RequiredCounts = GetRequiredCountsForProduct(Product);
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
bool ASAS_ProductionBuilding::AddProductToQueue(USAS_GameDataAsset* Product)
{
    if (!Product || !RecipesMap.Contains(Product))
        return false;

    ProductionQueue.Add(Product);
    TMap<TObjectPtr<USAS_GameDataAsset>, int32> RequiredCounts = GetTotalRequiredCounts();
    UE_LOG(LogTemp, Warning, TEXT("Required Counts: %d"), RequiredCounts.Num());
    if (RequiredCounts.Num() > 0)
    {
		FProductionRequirements requirements = { RequiredCounts };

		//Create a job with the requirements and add it to the job manager
        ASAS_GameState* GameState = GetWorld()->GetGameState<ASAS_GameState>();
        if (!GameState)
        {
            return false;
        }
        FSAS_ResourceDeliveryRequest2 ResourceDeliveryInformation;
		ResourceDeliveryInformation.BuildSite = this;
		ResourceDeliveryInformation.ResourceCost = RequiredCounts;
         bool found = false;
		// Check for an existing job first and update it instead of creating a new one every time.
        for (auto& activeJob : ActiveLogisticsJobs) {
			USAS_LMJ_DeliverItem* DeliverJob = Cast<USAS_LMJ_DeliverItem>(activeJob);

            // I assume a completed job will be automatically removed so don't need to check for that.
            if (!DeliverJob)
				continue;

            USAS_GameDataAsset* ItemDefinition = Cast<USAS_GameDataAsset>(DeliverJob->GetItemDefinition());
            if (!ItemDefinition)
                continue;

            if (ItemDefinition == Product)
            {
                // Not seeing a built in way to update an existing job outside it's definition with out editing it.
                //DeliverJob->
                return true;
			}
            
        }

        TArray<USAS_LogisticsMasterJob*> NewGameJobs = GameState->GetLogisticsManager()->CreateBuildSiteResourceDeliveryJob2(ResourceDeliveryInformation);

            
		

        // Log the required counts for debugging
        UE_LOG(LogTemp, Warning, TEXT("Total Required Counts after adding product to queue:"));
        for (auto& Pair : RequiredCounts)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: %d"), *Pair.Key->GetName(), Pair.Value);
        }
    }


    return true;
}

void ASAS_ProductionBuilding::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.Property &&
        PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ASAS_ProductionBuilding, Recipes))
    {
        TSet<TObjectPtr<USAS_GameDataAsset>> SeenProducts;
        bool bHasDuplicates = false;
        
        // First pass: detect duplicates
        for (int i = 0; i < Recipes.Num(); ++i)
        {
            TObjectPtr<USAS_RecipeData>& Recipe = Recipes[i];

            if (Recipe && Recipe->Product)
            {
                if (SeenProducts.Contains(Recipe->Product))
                {
                    // Log error and show notification
                    UE_LOG(LogTemp, Error, TEXT("Duplicate Product found in RecipesArray. Each Product must be unique."));
                    FNotificationInfo Info(FText::FromString("Error: Duplicate Product found in RecipesArray. Each Product must be unique."));

                    Info.bUseLargeFont = false;
                    Info.bFireAndForget = true;
                    FSlateNotificationManager::Get().AddNotification(Info);
                    Recipe = nullptr;  // Set to null to make it unselected/empty
                    break;
                }
                SeenProducts.Add(Recipe->Product);
            }
        }       

        // Always update the map
        BuildRecipesMap();
    }
}

void ASAS_ProductionBuilding::BuildRecipesMap()
{
    RecipesMap.Empty();
    for (const TObjectPtr<USAS_RecipeData>& Recipe : Recipes)
    {
        if (Recipe && Recipe->Product)
        {
            RecipesMap.Add(Recipe->Product, Recipe);
        }
    }
}

