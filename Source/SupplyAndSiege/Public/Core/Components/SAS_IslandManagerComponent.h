

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_IslandManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_IslandManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USAS_IslandManagerComponent();

	static void CleanupGUIDAssignmentQueue();
	FGuid GetIslandID();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	static TArray<TWeakObjectPtr<USAS_IslandManagerComponent>> GUIDAssignmentQueue;
		
};
