

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_IslandManagerComponent.generated.h"

class USAS_IslandComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_IslandManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_IslandManagerComponent();

	UFUNCTION(BlueprintCallable, Category = "Island")
	int32 GetHealth(USAS_IslandComponent* IslandComponent) const;
	
	UFUNCTION(BlueprintCallable, Category = "Island")
	bool IsDepleted(const int32 islandHealth) const;

	UFUNCTION(BlueprintCallable, Category = "Island")
	void AddIslandToQueue(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "Island")
	void RemoveIslandFromQueue(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "Island")
	void CleanupIslandQueue();
	
	UFUNCTION(BlueprintCallable, Category = "Island")
	TArray<USAS_IslandComponent*> GetIslandComponents() const;

	UFUNCTION(BlueprintCallable, Category = "Island")
	void CheckIslandHealth(USAS_IslandComponent* IslandComponent);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<USAS_IslandComponent>> IslandQueue;
		
};
