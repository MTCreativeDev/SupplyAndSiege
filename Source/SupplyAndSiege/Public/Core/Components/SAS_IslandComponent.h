

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
	FGuid GetIslandID() const;

	UFUNCTION(BlueprintCallable, Category = "Island")
	int32 GetHealth() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Island")
	FGuid IslandID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "Island")
	int32 Health = 100;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
};
