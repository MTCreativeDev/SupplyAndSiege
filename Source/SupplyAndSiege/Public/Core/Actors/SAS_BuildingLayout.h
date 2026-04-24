

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAS_BuildingLayout.generated.h"


class UBoxComponent;
class USAS_BuildingDefinitionData;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_BuildingLayout : public AActor
{
	GENERATED_BODY()
	
public:	
	ASAS_BuildingLayout();

	virtual void ApplyDefinitionToComponents();

	void SetBuildingDefinition(USAS_BuildingDefinitionData* NewBuildingDefinition);

	UFUNCTION(BlueprintCallable, Category = "Building")
	FVector GetBestMoveToWorldLocation(const FVector& FromWorldLocation) const;



#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif



protected:


public:	

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Building")
	USceneComponent* SceneRoot;

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Building")
	UBoxComponent* BuildingAreaCollision;

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Building")
	UStaticMeshComponent* PrimaryMesh;

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Building")
	UStaticMeshComponent* SecondaryMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Building")
	USceneComponent* MoveToLocationsContainer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAS_Building", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<USAS_BuildingDefinitionData> BuildingDefinition = nullptr;



};
