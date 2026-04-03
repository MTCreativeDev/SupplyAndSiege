

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAS_FloatingIsland.generated.h"

UCLASS()
class SUPPLYANDSIEGE_API ASAS_FloatingIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASAS_FloatingIsland();
	UFUNCTION(BlueprintCallable, Category = "Island")
	FGuid GetIslandID();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Island")
	UStaticMeshComponent* FloatingIslandMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	FGuid IslandID;

};
