
#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_BuildingLayout.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_SelectableBuilding.generated.h"

class UBoxComponent;
class USAS_UnitInformationComponent;
class UStaticMeshComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_SelectableBuilding : public ASAS_BuildingLayout
{
	GENERATED_BODY()
	
public:	
	ASAS_SelectableBuilding();

	UFUNCTION(BlueprintCallable, Category = "SAS_Unit")
	void DestroySelf();

	UFUNCTION(BlueprintCallable, Category = "Selectable_Building")
	const TArray<FVector> GetMoveToLocations() const;



protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Resource_Depot")
	bool bShowMoveToLocations = true;

	void RebuildMoveToLocations();



public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAS_Unit")
	ESAS_Team AssignTeamOnSpawn = ESAS_Team::None;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USAS_UnitInformationComponent* UnitInformationComponent;

	TArray<FVector> MoveToLocations_World;


};
