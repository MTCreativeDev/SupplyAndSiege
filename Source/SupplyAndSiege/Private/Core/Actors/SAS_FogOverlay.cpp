#include "Core/Actors/SAS_FogOverlay.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ASAS_FogOverlay::ASAS_FogOverlay()
{
	PrimaryActorTick.bCanEverTick = false;

	OverlayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OverlayMesh"));
	SetRootComponent(OverlayMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		OverlayMesh->SetStaticMesh(PlaneMesh.Object);
	}

	OverlayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OverlayMesh->SetGenerateOverlapEvents(false);
	OverlayMesh->SetCastShadow(false);
	OverlayMesh->SetRenderInMainPass(true);
}

void ASAS_FogOverlay::BeginPlay()
{
	Super::BeginPlay();

	if (OverlayMaterial)
	{
		OverlayMID = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
		if (OverlayMID && OverlayMesh)
		{
			OverlayMesh->SetMaterial(0, OverlayMID);
			if (CurrentMaskTexture)
			{
				OverlayMID->SetTextureParameterValue(TEXT("MaskTexture"), CurrentMaskTexture);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("ASAS_FogOverlay '%s' has no OverlayMaterial set on its BP defaults; fog overlay will not render."),
			*GetName());
	}
}

void ASAS_FogOverlay::ConfigureOverlay(const FVector2D& WorldOrigin, const FVector2D& WorldExtent, float ZHeight)
{
	SetActorLocation(FVector(WorldOrigin.X, WorldOrigin.Y, ZHeight));
	SetActorRotation(FRotator::ZeroRotator);

	if (OverlayMesh && OverlayMesh->GetStaticMesh())
	{
		const FBoxSphereBounds MeshBounds = OverlayMesh->GetStaticMesh()->GetBounds();
		const FVector LocalHalfSize = MeshBounds.BoxExtent;
		const float ScaleX = LocalHalfSize.X > 0.f ? WorldExtent.X / LocalHalfSize.X : 1.f;
		const float ScaleY = LocalHalfSize.Y > 0.f ? WorldExtent.Y / LocalHalfSize.Y : 1.f;
		OverlayMesh->SetRelativeScale3D(FVector(ScaleX, ScaleY, 1.f));
	}
}

void ASAS_FogOverlay::SetMaskTexture(UTextureRenderTarget2D* MaskRT)
{
	CurrentMaskTexture = MaskRT;

	if (OverlayMID && MaskRT)
	{
		OverlayMID->SetTextureParameterValue(TEXT("MaskTexture"), MaskRT);
	}
}
