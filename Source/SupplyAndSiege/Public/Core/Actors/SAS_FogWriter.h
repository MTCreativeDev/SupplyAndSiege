// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAS_FogWriter.generated.h"

class UDecalComponent;
class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class UMaterialInterface;

/**
 * Decal actor spawned per local player by USAS_FogOfWarClientComponent.
 * Its M_FogWriter material has "Output Virtual Textures" enabled, which is what
 * makes this primitive write into RVT_FogOfWar during the RVT pass. The RVT bounds
 * themselves are defined by an ARuntimeVirtualTextureVolume placed in the level.
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_FogWriter : public AActor
{
	GENERATED_BODY()

public:
	ASAS_FogWriter();

	/** Set the texture sampled by the fog-writer material as the mask source.
	 *  Called by USAS_FogOfWarClientComponent each tick after ping-pong swap. */
	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void SetMaskTexture(UTextureRenderTarget2D* MaskRT);

	/** Configure the decal world-XY footprint. Called once on spawn. */
	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void ConfigureDecalExtent(const FVector2D& WorldExtent);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FogOfWar")
	TObjectPtr<UDecalComponent> Decal;

	/** Set in the Blueprint subclass: the M_FogWriter material (parent of the dynamic instance).
	 *  The material must have "Output Virtual Textures" enabled targeting RVT_FogOfWar — that
	 *  flag is what makes this primitive write into the RVT during the RVT pass. */
	UPROPERTY(EditDefaultsOnly, Category = "FogOfWar")
	TObjectPtr<UMaterialInterface> WriterMaterial;

	/** Dynamic material instance used to set the mask texture parameter at runtime. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> WriterMID;
};
