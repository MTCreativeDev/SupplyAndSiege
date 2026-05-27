// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAS_FogWriter.generated.h"

class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class UMaterialInterface;
class URuntimeVirtualTexture;
class UStaticMeshComponent;

/**
 * RVT writer actor spawned per local player by USAS_FogOfWarClientComponent.
 * It uses a hidden mesh because RVT writes are driven through primitive components.
 * The RVT bounds themselves are defined by an ARuntimeVirtualTextureVolume found or spawned by the client component.
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

	/** Configure the writer mesh world-XY half-size. Called once on spawn. */
	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void ConfigureDecalExtent(const FVector2D& WorldExtent);

	URuntimeVirtualTexture* GetFogRuntimeVirtualTexture() const { return FogRuntimeVirtualTexture; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FogOfWar")
	TObjectPtr<UStaticMeshComponent> WriterMesh;

	/** Set in the Blueprint subclass: the M_FogWriter material (parent of the dynamic instance).
	 *  The material must have "Output Virtual Textures" enabled targeting RVT_FogOfWar. */
	UPROPERTY(EditDefaultsOnly, Category = "FogOfWar")
	TObjectPtr<UMaterialInterface> WriterMaterial;

	/** Set in the Blueprint subclass: the RVT this writer draws into. */
	UPROPERTY(EditDefaultsOnly, Category = "FogOfWar")
	TObjectPtr<URuntimeVirtualTexture> FogRuntimeVirtualTexture;

	/** Dynamic material instance used to set the mask texture parameter at runtime. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> WriterMID;
};
