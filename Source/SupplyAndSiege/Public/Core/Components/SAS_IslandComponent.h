

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

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	FGuid GetIslandID() const;

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	int32 GetCaeliumDeposits() const;

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	int32 MineCaeliumDeposits();

	UFUNCTION(BlueprintCallable, Category = "SAS_Island")
	void PlayFallAnimation();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Island")
	FGuid IslandID;

public:	
	// TODO - Update to finalized value for starting Caelium of an Island
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SAS_Island")
	int32 CaeliumRemaining = 100;

	// TODO - Update with finalized value of how much caelium should be removed when being mined.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SAS_Island")
	int32 CaeliumMined = 1;
};
