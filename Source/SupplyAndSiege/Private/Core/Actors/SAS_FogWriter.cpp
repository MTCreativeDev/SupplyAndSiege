// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/SAS_FogWriter.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"
#include "VT/RuntimeVirtualTexture.h"
#include "VT/RuntimeVirtualTextureEnum.h"

ASAS_FogWriter::ASAS_FogWriter()
{
	PrimaryActorTick.bCanEverTick = false;

	WriterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WriterMesh"));
	SetRootComponent(WriterMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		WriterMesh->SetStaticMesh(PlaneMesh.Object);
	}

	WriterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WriterMesh->SetCastShadow(false);
	WriterMesh->SetRenderInMainPass(false);
	WriterMesh->VirtualTextureRenderPassType = ERuntimeVirtualTextureMainPassType::Never;
}

void ASAS_FogWriter::BeginPlay()
{
	Super::BeginPlay();

	if (WriterMaterial)
	{
		WriterMID = UMaterialInstanceDynamic::Create(WriterMaterial, this);
		if (WriterMID && WriterMesh)
		{
			WriterMesh->SetMaterial(0, WriterMID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("ASAS_FogWriter '%s' has no WriterMaterial set on its BP defaults; fog will not render."),
			*GetName());
	}

	if (WriterMesh)
	{
		WriterMesh->SetRenderInMainPass(false);
		WriterMesh->VirtualTextureRenderPassType = ERuntimeVirtualTextureMainPassType::Never;
		WriterMesh->RuntimeVirtualTextures.Reset();
		if (FogRuntimeVirtualTexture)
		{
			WriterMesh->RuntimeVirtualTextures.Add(FogRuntimeVirtualTexture);
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("ASAS_FogWriter '%s' has no FogRuntimeVirtualTexture set; fog will not write to RVT."),
				*GetName());
		}

		WriterMesh->MarkRenderStateDirty();
		UE_LOG(LogTemp, Log,
			TEXT("ASAS_FogWriter '%s' configured: MainPass=0, RVTCount=%d, Scale=%s."),
			*GetName(), WriterMesh->RuntimeVirtualTextures.Num(), *WriterMesh->GetComponentScale().ToString());
	}
}

void ASAS_FogWriter::SetMaskTexture(UTextureRenderTarget2D* MaskRT)
{
	if (WriterMID && MaskRT)
	{
		WriterMID->SetTextureParameterValue(TEXT("MaskTexture"), MaskRT);
	}
}

void ASAS_FogWriter::ConfigureDecalExtent(const FVector2D& WorldExtent)
{
	if (WriterMesh && WriterMesh->GetStaticMesh())
	{
		const FBoxSphereBounds MeshBounds = WriterMesh->GetStaticMesh()->GetBounds();
		const FVector LocalHalfSize = MeshBounds.BoxExtent;
		const float ScaleX = LocalHalfSize.X > 0.f ? WorldExtent.X / LocalHalfSize.X : 1.f;
		const float ScaleY = LocalHalfSize.Y > 0.f ? WorldExtent.Y / LocalHalfSize.Y : 1.f;
		WriterMesh->SetRelativeScale3D(FVector(ScaleX, ScaleY, 1.f));
		WriterMesh->MarkRenderStateDirty();
		UE_LOG(LogTemp, Display,
			TEXT("ASAS_FogWriter '%s' extent configured: HalfExtent=%s, MeshLocalHalfSize=%s, Scale=%s."),
			*GetName(), *WorldExtent.ToString(), *LocalHalfSize.ToString(), *WriterMesh->GetComponentScale().ToString());
	}
}
