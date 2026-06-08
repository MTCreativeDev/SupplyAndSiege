

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceKey.h"
#include "SAS_UnitControlComponent.generated.h"

class USAS_UnitInformationComponent;
class USAS_ResourceTypeData;

UCLASS()
class SUPPLYANDSIEGE_API USAS_UnitControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USAS_UnitControlComponent();

	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual bool GroundSelectIsMove() const;

	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual bool ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const;

	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual bool CanReceiveAttackOrder() const;


	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual void HandleGroundRightClick(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual void HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation);

	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual void HandleAttackUnitOrder(USAS_UnitInformationComponent* TargetUnitInformationComponent, const FVector& ClickLocation);


	UFUNCTION(BlueprintCallable, Category = "Unit Control")
	virtual bool CanReceivePlayerCommands() const;

protected:

	virtual void BeginPlay() override;

public:	

protected:
	UPROPERTY()
	TObjectPtr<USAS_UnitInformationComponent> UnitInformation = nullptr;
			
};
