

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_IslandComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_IslandComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_IslandComponent();
	UFUNCTION(BlueprintCallable, Category = "Island")
	FGuid GetIslandID();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Island")
	UStaticMeshComponent* IslandMesh;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	FGuid IslandID;
		
};
