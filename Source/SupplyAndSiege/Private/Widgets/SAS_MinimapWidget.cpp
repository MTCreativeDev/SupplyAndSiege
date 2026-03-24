// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SAS_MinimapWidget.h"
#include "Core/Controllers/SAS_PlayerController.h"
#include "Core/Components/SAS_UnitManagerComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"


// --- Construction ---

USAS_MinimapWidget::USAS_MinimapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WorldOrigin(FVector2D::ZeroVector)
	, WorldExtent(10000.0, 10000.0)
	, CaptureHeight(50000.f)
	, RenderTargetResolution(512)
	, ZoomLevel(1.f)
	, MinZoom(0.5f)
	, MaxZoom(5.f)
	, ZoomStep(0.25f)
{
}

// --- Lifecycle ---

void USAS_MinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerController = Cast<ASAS_PlayerController>(GetOwningPlayer());
	if (!PlayerController)
	{
		HOOK_LOG(EHookLogType::Error, TEXT("MinimapWidget failed to get PlayerController"));
		return;
	}

	UnitManagerComponent = PlayerController->FindComponentByClass<USAS_UnitManagerComponent>();
	if (!UnitManagerComponent)
	{
		HOOK_LOG(EHookLogType::Warning, TEXT("MinimapWidget could not find UnitManagerComponent"));
	}

	SpawnCaptureActor();

	if (MiniMapImage && MinimapRenderTarget)
	{
		MiniMapImage->SetBrushResourceObject(MinimapRenderTarget);
	}

	HOOK_LOG(EHookLogType::Success, TEXT("MinimapWidget initialized"));
}

void USAS_MinimapWidget::NativeDestruct()
{
	if (CaptureActor)
	{
		CaptureActor->Destroy();
		CaptureActor = nullptr;
		CaptureComponent = nullptr;
		HOOK_LOG(EHookLogType::Info, TEXT("MinimapWidget capture actor destroyed"));
	}

	Super::NativeDestruct();
}

void USAS_MinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateCaptureTransform();

	TArray<FVector2D> FrustumCorners;
	GatherCameraFrustum(FrustumCorners);
	BP_UpdateCameraFrustum(FrustumCorners);
}

// --- Zoom ---

void USAS_MinimapWidget::ZoomIn()
{
	ZoomLevel += ZoomStep;
	ClampZoom();
	HOOK_LOG(EHookLogType::Info, FString::Printf(TEXT("Minimap zoom in: %.2f"), ZoomLevel));
}

void USAS_MinimapWidget::ZoomOut()
{
	ZoomLevel -= ZoomStep;
	ClampZoom();
	HOOK_LOG(EHookLogType::Info, FString::Printf(TEXT("Minimap zoom out: %.2f"), ZoomLevel));
}

void USAS_MinimapWidget::ClampZoom()
{
	ZoomLevel = FMath::Clamp(ZoomLevel, MinZoom, MaxZoom);
}

// --- Capture Actor ---

void USAS_MinimapWidget::SpawnCaptureActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		HOOK_LOG(EHookLogType::Error, TEXT("SpawnCaptureActor failed: no World"));
		return;
	}

	MinimapRenderTarget = NewObject<UTextureRenderTarget2D>(this);
	MinimapRenderTarget->InitAutoFormat(RenderTargetResolution, RenderTargetResolution);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CaptureActor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!CaptureActor)
	{
		HOOK_LOG(EHookLogType::Error, TEXT("SpawnCaptureActor failed: could not spawn actor"));
		return;
	}

	/* AActor::StaticClass() spawns with no root component, so create one */
	USceneComponent* Root = NewObject<USceneComponent>(CaptureActor);
	Root->RegisterComponent();
	CaptureActor->SetRootComponent(Root);

	CaptureComponent = NewObject<USceneCaptureComponent2D>(CaptureActor);
	CaptureComponent->SetupAttachment(Root);
	CaptureComponent->RegisterComponent();

	CaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	CaptureComponent->TextureTarget = MinimapRenderTarget;
	CaptureComponent->bCaptureEveryFrame = true;
	CaptureComponent->bCaptureOnMovement = false;

	UpdateCaptureTransform();

	HOOK_LOG(EHookLogType::Success, FString::Printf(TEXT("Capture actor spawned at height %.0f, OrthoWidth %.0f, RT %dx%d"),
		CaptureHeight, CaptureComponent->OrthoWidth, RenderTargetResolution, RenderTargetResolution));
}

void USAS_MinimapWidget::UpdateCaptureTransform()
{
	if (!CaptureActor || !CaptureComponent) return;

	CaptureActor->SetActorLocationAndRotation(
		FVector(WorldOrigin.X, WorldOrigin.Y, CaptureHeight),
		FRotator(-90.f, 0.f, 0.f)
	);

	float BaseWidth = WorldExtent.X * 2.f;
	CaptureComponent->OrthoWidth = BaseWidth / ZoomLevel;
}

// --- Camera Frustum ---

void USAS_MinimapWidget::GatherCameraFrustum(TArray<FVector2D>& OutCorners) const
{
	if (!PlayerController) return;

	int32 SizeX = 0;
	int32 SizeY = 0;
	PlayerController->GetViewportSize(SizeX, SizeY);
	if (SizeX == 0 || SizeY == 0) return;

	const float Corners[4][2] =
	{
		{ 0.f,           0.f },
		{ (float)SizeX,  0.f },
		{ (float)SizeX,  (float)SizeY },
		{ 0.f,           (float)SizeY }
	};

	OutCorners.Reserve(4);
	for (int32 i = 0; i < 4; ++i)
	{
		FVector WorldPos;
		if (DeprojectCornerToGroundPlane(Corners[i][0], Corners[i][1], WorldPos))
		{
			OutCorners.Add(WorldToMinimap(WorldPos));
		}
	}
}

bool USAS_MinimapWidget::DeprojectCornerToGroundPlane(float ScreenX, float ScreenY, FVector& OutWorldPos) const
{
	FVector WorldPos;
	FVector WorldDir;
	if (!PlayerController->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldPos, WorldDir))
	{
		return false;
	}

	/* Avoid division by near-zero or rays pointing away from the ground */
	if (FMath::Abs(WorldDir.Z) < KINDA_SMALL_NUMBER) return false;

	float T = -WorldPos.Z / WorldDir.Z;
	if (T < 0.f) return false;

	OutWorldPos = WorldPos + WorldDir * T;
	return true;
}

// --- Coordinate Conversion ---

FVector2D USAS_MinimapWidget::WorldToMinimap(const FVector& WorldLocation) const
{
	if (!CaptureComponent) return FVector2D::ZeroVector;

	FVector CapturePos = CaptureActor->GetActorLocation();
	float HalfWidth = CaptureComponent->OrthoWidth * 0.5f;

	/* Use OrthoWidth for both axes since the render target is square */
	float X = (WorldLocation.X - (CapturePos.X - HalfWidth)) / (HalfWidth * 2.f);
	float Y = (WorldLocation.Y - (CapturePos.Y - HalfWidth)) / (HalfWidth * 2.f);

	return FVector2D(
		FMath::Clamp(X, 0.f, 1.f),
		FMath::Clamp(Y, 0.f, 1.f)
	);
}

// --- Blueprint Event Defaults ---

void USAS_MinimapWidget::BP_UpdateCameraFrustum_Implementation(const TArray<FVector2D>& FrustumCorners)
{
	// handled in BP
}
