
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_SelectableBuilding.generated.h"

class UBoxComponent;
class USAS_UnitInformationComponent;
class UStaticMeshComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_SelectableBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	ASAS_SelectableBuilding();

	UFUNCTION(BlueprintCallable, Category = "SAS_Unit")
	void DestroySelf();

	UFUNCTION(BlueprintCallable, Category = "Selectable_Building")
	const TArray<FVector> GetMoveToLocations() const;

	float GetDefaultHalfHeight();

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

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Unit")
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USceneComponent* SceneRoot;

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Unit")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Placement")
	bool bSnapToGroundOnConstruction = false;

	float GroundTraceUp = 5000.f;
	float GroundTraceDown = 5000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selectable_Building")
	USceneComponent* MoveToLocationsContainer;

	TArray<FVector> MoveToLocations_World;


};
