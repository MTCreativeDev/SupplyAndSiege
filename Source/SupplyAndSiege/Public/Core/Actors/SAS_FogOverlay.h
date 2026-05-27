#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAS_FogOverlay.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UStaticMeshComponent;
class UTextureRenderTarget2D;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_FogOverlay : public AActor
{
	GENERATED_BODY()

public:
	ASAS_FogOverlay();

	/** WorldExtent is a world-XY half extent; e.g. (10000, 10000) covers -10000..10000 from WorldOrigin. */
	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void ConfigureOverlay(const FVector2D& WorldOrigin, const FVector2D& WorldExtent, float ZHeight);

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void SetMaskTexture(UTextureRenderTarget2D* MaskRT);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FogOfWar")
	TObjectPtr<UStaticMeshComponent> OverlayMesh;

	UPROPERTY(EditDefaultsOnly, Category = "FogOfWar")
	TObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> OverlayMID;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> CurrentMaskTexture;
};
