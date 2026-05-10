// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SAS_Enumerators.h"
#include "SAS_FogOfWarClientComponent.generated.h"

class APlayerController;
class ASAS_FogWriter;
class USAS_VisionManagerComponent;
class UCanvasRenderTarget2D;
class UTexture2D;
class UMaterialParameterCollection;
class UMaterialInterface;

/**
 * Client-local fog-of-war painter. Attaches to ASAS_PlayerController and runs only on the
 * local controller. Reads source registry from the GameState's USAS_VisionManagerComponent
 * filtered to the local player's team, paints into a ping-pong UCanvasRenderTarget2D
 * (RGBA8: R=soft current, G=explored memory, B=hard current), and pushes the result to
 * a runtime-spawned ASAS_FogWriter whose material writes into RVT_FogOfWar.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SUPPLYANDSIEGE_API USAS_FogOfWarClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAS_FogOfWarClientComponent();

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	UCanvasRenderTarget2D* GetVisionMaskRT() const;

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	FVector2D WorldToMaskUV(const FVector& WorldLocation) const;

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	FVector2D GetWorldOrigin() const { return WorldOrigin; }

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	FVector2D GetWorldExtent() const { return WorldExtent; }

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void RequestImmediateRedraw();

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	void NotifyViewingTeamChanged(ESAS_Team NewTeam);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Periodic mask paint pass driven by UpdateTimer. */
	void UpdateMask();

	/** Resolves the local viewing team from the owning controller's UnitManager. */
	ESAS_Team ResolveViewingTeam() const;

	/** Returns a pointer to the GameState's vision manager, or nullptr if not yet present. */
	USAS_VisionManagerComponent* GetVisionManager() const;

	/** First-time setup: create RTs, spawn fog writer, push MPC params. Idempotent. */
	void InitializeIfReady();

	/** Paints the next ping-pong RT from sources for the given team, copying G from PreviousRT. */
	void PaintMaskInto(UCanvasRenderTarget2D* TargetRT, UCanvasRenderTarget2D* PreviousRT, ESAS_Team ViewingTeam);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Bounds")
	FVector2D WorldOrigin = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Bounds")
	FVector2D WorldExtent = FVector2D(10000.f, 10000.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Quality")
	int32 RTResolution = 1024;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Update")
	float UpdateIntervalSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Brushes")
	TObjectPtr<UTexture2D> SoftBrushTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Brushes")
	TObjectPtr<UTexture2D> HardBrushTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Materials")
	TObjectPtr<UMaterialInterface> PaintSoftMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Materials")
	TObjectPtr<UMaterialInterface> PaintHardMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|Materials")
	TObjectPtr<UMaterialInterface> CopyGMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|MPC")
	TObjectPtr<UMaterialParameterCollection> FogOfWarMPC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FogOfWar|FogWriter")
	TSubclassOf<ASAS_FogWriter> FogWriterClass;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> RT_A;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasRenderTarget2D> RT_B;

	UPROPERTY(Transient)
	TObjectPtr<ASAS_FogWriter> OwnedFogWriter;

	int32 ActiveRTIndex = 0;
	bool bReady = false;
	bool bDependencyWarningLogged = false;
	ESAS_Team CachedViewingTeam = ESAS_Team::None;

	FTimerHandle UpdateTimer;
	FTimerHandle InitRetryTimer;
};
