

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_WinLossManagerComponent.generated.h"

struct FSAS_BuildingWinRequirement;
class ASAS_BuildingLayout;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingWinRequirementsChanged, const TArray<FSAS_BuildingWinRequirement>&, BuildingRequirements);

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_WinLossManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_WinLossManagerComponent();
	void NotifyBuildingCompleted(ASAS_BuildingLayout* CompletedBuilding);
	void NotifyAllVillagersDead();

	//Dispatchers

	UPROPERTY(BlueprintAssignable, Category = "Win Loss")
	FOnBuildingWinRequirementsChanged OnBuildingWinRequirementsChanged;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Win Loss")
	void HandleGameWon();

	UFUNCTION(BlueprintImplementableEvent, Category = "Win Loss")
	void HandleGameLost();

private:
	void CheckWinCondition();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Win Condition")
	TArray<FSAS_BuildingWinRequirement> BuildingRequirements;

private:
	bool bGameEnded = false;

		
};
