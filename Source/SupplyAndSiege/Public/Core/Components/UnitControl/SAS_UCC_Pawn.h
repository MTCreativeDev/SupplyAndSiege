

#pragma once

#include "CoreMinimal.h"
#include "Core/Components/SAS_UnitControlComponent.h"
#include "SAS_UCC_Pawn.generated.h"

class USAS_UnitInformationComponent;

struct FGameplayTag;

UENUM(BlueprintType)
enum class ESAS_PawnControlState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Moving	UMETA(DisplayName = "Moving"),
	Attacking	UMETA(DisplayName = "Attacking"),
	Stunned	UMETA(DisplayName = "Stunned")
};

UCLASS(BlueprintType, Blueprintable)
class SUPPLYANDSIEGE_API USAS_UCC_Pawn : public USAS_UnitControlComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Pawn Unit Control")
	ESAS_PawnControlState GetCurrentPawnControlState() const { return CurrentPawnControlState; }

	UFUNCTION(BlueprintCallable, Category = "Pawn Unit Control")
	FVector GetCurrentMoveTarget() const { return CurrentMoveTarget; }

	UFUNCTION(BlueprintCallable, Category = "Pawn Unit Control")
	USAS_UnitInformationComponent* GetCurrentAttackTarget() const { return CurrentAttackTarget; }


	virtual bool GroundSelectIsMove() const override;
	virtual void HandleGroundRightClick(const FVector& WorldLocation) override;
	virtual bool ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const override;

protected:

	void BeginMove_Internal(const FVector& WorldLocation);
	void SendStateTreeEvent(const FGameplayTag& EventTag);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn Unit Control")
	ESAS_PawnControlState CurrentPawnControlState = ESAS_PawnControlState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn Unit Control")
	FVector CurrentMoveTarget = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn Unit Control")
	USAS_UnitInformationComponent* CurrentAttackTarget;

};
