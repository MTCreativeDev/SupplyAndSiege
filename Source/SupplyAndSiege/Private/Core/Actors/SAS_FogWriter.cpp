// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Actors/SAS_FogWriter.h"

#include "Components/DecalComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

ASAS_FogWriter::ASAS_FogWriter()
{
	PrimaryActorTick.bCanEverTick = false;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	SetRootComponent(Decal);

	// Top-down projection: rotate so the decal projects straight down the world Z axis.
	Decal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	// Default extent (overridden in ConfigureDecalExtent on spawn). Y/Z map to world XY half-extents
	// after the -90° pitch; X is the projection-depth slab.
	Decal->DecalSize = FVector(2048.f, 5000.f, 5000.f);

	Decal->SortOrder = -100;
}

void ASAS_FogWriter::BeginPlay()
{
	Super::BeginPlay();

	if (WriterMaterial)
	{
		WriterMID = UMaterialInstanceDynamic::Create(WriterMaterial, this);
		if (WriterMID && Decal)
		{
			Decal->SetDecalMaterial(WriterMID);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error,
			TEXT("ASAS_FogWriter '%s' has no WriterMaterial set on its BP defaults; fog will not render."),
			*GetName());
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
	if (Decal)
	{
		// X axis on the decal is the projection-depth slab; Y/Z become world XY half-extents
		// after the -90° pitch in the constructor. Use half-extents because DecalSize is
		// half-size by Unreal's convention.
		Decal->DecalSize = FVector(2048.f, WorldExtent.Y * 0.5f, WorldExtent.X * 0.5f);
	}
}
