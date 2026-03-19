

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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	void ApplyDefinitionToComponents();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SAS_Building")
	TObjectPtr<USAS_BuildingDefinitionData> BuildingDefinition = nullptr;



};
