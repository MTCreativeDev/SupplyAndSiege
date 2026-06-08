

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "SAS_UnitSightComponent.generated.h"

class USphereComponent;
class USAS_UnitInformationComponent;
class USAS_VisibilityManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDetected, AActor*, EnemyActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyLost, AActor*, EnemyActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_UnitSightComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	USAS_UnitSightComponent();

	//Dispatchers

	UPROPERTY(BlueprintAssignable, Category = "UnitSight")
	FOnEnemyDetected OnEnemyDetected;

	UPROPERTY(BlueprintAssignable, Category = "UnitSight")
	FOnEnemyLost OnEnemyLost;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleSightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleSightEndOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool IsValidEnemy(AActor* OtherActor);

private:


public:	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnitSight")
	float SightRadius = 3000.f;

	UPROPERTY()
	TObjectPtr<USAS_UnitInformationComponent> OwnerUnitInfo;
		
	UPROPERTY()
	TArray<TObjectPtr<AActor>> VisibleEnemies;

	UPROPERTY()
	USAS_VisibilityManagerComponent* VisibilityManagerComponent;


};
