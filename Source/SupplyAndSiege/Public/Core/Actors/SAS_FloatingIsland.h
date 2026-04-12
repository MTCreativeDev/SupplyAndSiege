

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAS_FloatingIsland.generated.h"

class UBoxComponent;
class USAS_IslandComponent;
class USAS_IslandDefinitionData;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_FloatingIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	ASAS_FloatingIsland();

	virtual void ApplyDefinitionToComponents();

	void SetIslandDefinition(USAS_IslandDefinitionData* NewIslandDefinition);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Island")
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category = "SAS_Island")
	UStaticMeshComponent* IslandMesh;

	UPROPERTY(EditAnywhere, Category = "SAS_Island")
	USAS_IslandComponent* IslandLogic;

	UPROPERTY(EditDefaultsOnly, Category = "SAS_Island")
	UBoxComponent* IslandAreaCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAS_Island", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<USAS_IslandDefinitionData> IslandDefinition = nullptr;

public:

};
