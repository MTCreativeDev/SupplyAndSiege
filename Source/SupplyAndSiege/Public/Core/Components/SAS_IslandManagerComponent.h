

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

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	int32 GetHealth(USAS_IslandComponent* IslandComponent) const;
	
	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	bool IsDepleted(const int32 islandHealth) const;

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void AddIslandToQueue(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void RemoveIslandFromQueue(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void CleanupIslandQueue();
	
	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	TArray<USAS_IslandComponent*> GetIslandComponents() const;

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void CheckIslandHealth(USAS_IslandComponent* IslandComponent);
	
protected:
	virtual void BeginPlay() override;

public:	

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<USAS_IslandComponent>> IslandQueue;
		
};
