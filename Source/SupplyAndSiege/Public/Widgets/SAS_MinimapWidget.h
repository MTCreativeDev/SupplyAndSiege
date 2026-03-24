// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SAS_MinimapWidget.generated.h"


class ASAS_PlayerController;
class USAS_UnitManagerComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UImage;

UCLASS()
class SUPPLYANDSIEGE_API USAS_MinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USAS_MinimapWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- Blueprint Callable ---

	UFUNCTION(BlueprintCallable, Category = "Minimap|Zoom")
	void ZoomIn();

	UFUNCTION(BlueprintCallable, Category = "Minimap|Zoom")
	void ZoomOut();

protected:
	// --- Blueprint Events ---

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Minimap")
	void BP_UpdateCameraFrustum(const TArray<FVector2D>& FrustumCorners);
	virtual void BP_UpdateCameraFrustum_Implementation(const TArray<FVector2D>& FrustumCorners);

	// --- Bound Widgets ---

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> MiniMapImage;

	// --- Cached References ---

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ASAS_PlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USAS_UnitManagerComponent> UnitManagerComponent;

	// --- Configuration ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Bounds")
	FVector2D WorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Bounds")
	FVector2D WorldExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Bounds")
	float CaptureHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Bounds")
	int32 RenderTargetResolution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Zoom")
	float ZoomLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Zoom")
	float MinZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Zoom")
	float MaxZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Zoom")
	float ZoomStep;

private:
	// --- Internal Helpers ---

	void SpawnCaptureActor();
	void UpdateCaptureTransform();
	void GatherCameraFrustum(TArray<FVector2D>& OutCorners) const;
	FVector2D WorldToMinimap(const FVector& WorldLocation) const;
	bool DeprojectCornerToGroundPlane(float ScreenX, float ScreenY, FVector& OutWorldPos) const;
	void ClampZoom();

	// --- Internal State ---

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> MinimapRenderTarget;

	UPROPERTY()
	TObjectPtr<AActor> CaptureActor;

	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;
};
