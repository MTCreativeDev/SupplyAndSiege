

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAS_TimeManagerComponent.generated.h"

class ADirectionalLight;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_TimeManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_TimeManagerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	void ProgressTime();

protected:
	UPROPERTY()
	ADirectionalLight* DirectionalLight;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float TickInterval = .25f;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float DayLengthInSeconds= 180.f;

	float PitchIncrementPerTick = 0.f;

	float CurrentSunPitch = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float StartingPitch = 175.f;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float MaxPitch = 400.f;
		
};
