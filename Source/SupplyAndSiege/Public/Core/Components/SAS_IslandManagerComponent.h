

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
	int32 GetCaeliumDeposits(USAS_IslandComponent* IslandComponent) const;
	
	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	bool IsDepleted(const int32 IslandCaelium) const;

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void AddIslandToQueue(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void RemoveIslandFromQueue(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void CleanupIslandQueue();
	
	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	TArray<USAS_IslandComponent*> GetIslandComponents() const;

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void CheckIslandCaeliumDeposits(USAS_IslandComponent* IslandComponent);

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	bool CanIslandFloat(const int32 IslandCaelium) const;
	
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SAS_Island")
	int32 MinCaeliumToFloat = 10;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<USAS_IslandComponent>> IslandQueue;
		
};
