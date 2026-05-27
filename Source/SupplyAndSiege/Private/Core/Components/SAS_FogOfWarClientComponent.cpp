// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Components/SAS_FogOfWarClientComponent.h"

#include "Core/Actors/SAS_FogOverlay.h"
#include "Components/RuntimeVirtualTextureComponent.h"
#include "Core/Actors/SAS_FogWriter.h"
#include "Core/Components/SAS_VisionComponent.h"
#include "Core/Components/SAS_VisionManagerComponent.h"
#include "Core/Components/SAS_UnitManagerComponent.h"
#include "Core/SAS_GameState.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "TimerManager.h"
#include "VT/RuntimeVirtualTexture.h"
#include "VT/RuntimeVirtualTextureVolume.h"

namespace
{
	constexpr float FogRVTVolumeHeight = 1000.f;
}

USAS_FogOfWarClientComponent::USAS_FogOfWarClientComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAS_FogOfWarClientComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPC = Cast<APlayerController>(GetOwner());
	if (!OwningPC)
	{
		UE_LOG(LogTemp, Error,
			TEXT("USAS_FogOfWarClientComponent must be attached to an APlayerController; current owner is %s."),
			*GetNameSafe(GetOwner()));
		return;
	}

	if (!OwningPC->IsLocalController())
	{
		// Remote PCs (server's view of clients in MP) should not run client-local fog paint.
		return;
	}

	InitializeIfReady();

	if (!bReady)
	{
		// Schedule a periodic retry; dependencies (GameState manager, viewing team) may
		// come up after this BeginPlay runs.
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				InitRetryTimer, this, &USAS_FogOfWarClientComponent::InitializeIfReady, 0.5f, true);
		}
	}
}

void USAS_FogOfWarClientComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(UpdateTimer);
		TM.ClearTimer(InitRetryTimer);
	}

	if (OwnedFogWriter)
	{
		OwnedFogWriter->Destroy();
		OwnedFogWriter = nullptr;
	}
	if (OwnedFogOverlay)
	{
		OwnedFogOverlay->Destroy();
		OwnedFogOverlay = nullptr;
	}
	if (OwnedFogVolume)
	{
		OwnedFogVolume->Destroy();
		OwnedFogVolume = nullptr;
	}

	RT_A = nullptr;
	RT_B = nullptr;
	FogVirtualTextureComponent = nullptr;

	Super::EndPlay(EndPlayReason);
}

UCanvasRenderTarget2D* USAS_FogOfWarClientComponent::GetVisionMaskRT() const
{
	return ActiveRTIndex == 0 ? RT_A : RT_B;
}

FVector2D USAS_FogOfWarClientComponent::WorldToMaskUV(const FVector& WorldLocation) const
{
	if (WorldExtent.X <= 0.f || WorldExtent.Y <= 0.f)
	{
		return FVector2D::ZeroVector;
	}

	const FVector2D FullExtent = WorldExtent * 2.f;
	return FVector2D(
		((WorldLocation.X - WorldOrigin.X) / FullExtent.X) + 0.5f,
		((WorldLocation.Y - WorldOrigin.Y) / FullExtent.Y) + 0.5f);
}

void USAS_FogOfWarClientComponent::RequestImmediateRedraw()
{
	if (bReady)
	{
		UpdateMask();
	}
}

void USAS_FogOfWarClientComponent::NotifyViewingTeamChanged(ESAS_Team NewTeam)
{
	CachedViewingTeam = NewTeam;

	// Memory does not transfer between teams. Clear both ping-pong targets.
	if (RT_A) { UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_A, FLinearColor::Black); }
	if (RT_B) { UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_B, FLinearColor::Black); }

	RequestImmediateRedraw();
}

ESAS_Team USAS_FogOfWarClientComponent::ResolveViewingTeam() const
{
	APlayerController* OwningPC = Cast<APlayerController>(GetOwner());
	if (!OwningPC)
	{
		return ESAS_Team::None;
	}
	USAS_UnitManagerComponent* UMC = OwningPC->FindComponentByClass<USAS_UnitManagerComponent>();
	if (!UMC)
	{
		return ESAS_Team::None;
	}
	return UMC->AssignedTeam;
}

USAS_VisionManagerComponent* USAS_FogOfWarClientComponent::GetVisionManager() const
{
	if (UWorld* World = GetWorld())
	{
		if (ASAS_GameState* GS = World->GetGameState<ASAS_GameState>())
		{
			return GS->FindComponentByClass<USAS_VisionManagerComponent>();
		}
	}
	return nullptr;
}

void USAS_FogOfWarClientComponent::InitializeIfReady()
{
	if (bReady) { return; }

	USAS_VisionManagerComponent* Manager = GetVisionManager();
	const ESAS_Team Team = ResolveViewingTeam();

	if (!Manager || Team == ESAS_Team::None)
	{
		if (!bDependencyWarningLogged)
		{
			UE_LOG(LogTemp, Verbose,
				TEXT("USAS_FogOfWarClientComponent waiting for VisionManager (%s) and ViewingTeam (%d)."),
				Manager ? TEXT("present") : TEXT("missing"), static_cast<int32>(Team));
			bDependencyWarningLogged = true;
		}
		return;
	}

	if (!FogOfWarMPC || !PaintSoftMaterial || !PaintHardMaterial || !CopyGMaterial
		|| !SoftBrushTexture || !HardBrushTexture)
	{
		UE_LOG(LogTemp, Error,
			TEXT("USAS_FogOfWarClientComponent on %s is missing one or more authored assets ")
			TEXT("(MPC/materials/brushes). Disabling."),
			*GetNameSafe(GetOwner()));
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(InitRetryTimer);
		}
		return;
	}

	UWorld* World = GetWorld();
	if (!World) { return; }

	// Create ping-pong RTs (RGBA8 by default). Both cleared to black.
	RT_A = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
		this, UCanvasRenderTarget2D::StaticClass(), RTResolution, RTResolution);
	RT_B = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
		this, UCanvasRenderTarget2D::StaticClass(), RTResolution, RTResolution);
	if (RT_A) { UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_A, FLinearColor::Black); }
	if (RT_B) { UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_B, FLinearColor::Black); }

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (FogWriterClass)
	{
		URuntimeVirtualTexture* FogRVT = nullptr;
		if (const ASAS_FogWriter* WriterCDO = FogWriterClass->GetDefaultObject<ASAS_FogWriter>())
		{
			FogRVT = WriterCDO->GetFogRuntimeVirtualTexture();
		}
		EnsureRuntimeVirtualTextureVolume(World, FogRVT);

		// WorldOrigin is the playable-area center. The writer mesh is hidden from the main pass
		// and only renders to the RVT, so keep it inside the RVT volume near world Z=0.
		const FVector SpawnLoc(WorldOrigin.X, WorldOrigin.Y, 0.f);
		OwnedFogWriter = World->SpawnActor<ASAS_FogWriter>(
			FogWriterClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		if (OwnedFogWriter)
		{
			OwnedFogWriter->ConfigureDecalExtent(WorldExtent);
			OwnedFogWriter->SetMaskTexture(GetVisionMaskRT());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("USAS_FogOfWarClientComponent has no FogWriterClass; RVT fog writer/material integration will not run."));
	}

	if (FogOverlayClass)
	{
		const FVector OverlayLoc(WorldOrigin.X, WorldOrigin.Y, OverlayZHeight);
		OwnedFogOverlay = World->SpawnActor<ASAS_FogOverlay>(
			FogOverlayClass, OverlayLoc, FRotator::ZeroRotator, SpawnParams);
		if (OwnedFogOverlay)
		{
			OwnedFogOverlay->ConfigureOverlay(WorldOrigin, WorldExtent, OverlayZHeight);
			OwnedFogOverlay->SetMaskTexture(GetVisionMaskRT());
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("USAS_FogOfWarClientComponent failed to spawn FogOverlayClass %s."),
				*GetNameSafe(FogOverlayClass.Get()));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("USAS_FogOfWarClientComponent has no FogOverlayClass; visual fog overlay will not render."));
	}

	// Push lower-left/full-size bounds to the MPC so materials can compute UV with (WorldXY - MinXY) / FullExtent.
	const FVector2D MinXY = WorldOrigin - WorldExtent;
	const FVector2D FullExtent = WorldExtent * 2.f;
	UKismetMaterialLibrary::SetVectorParameterValue(this, FogOfWarMPC, TEXT("WorldOriginExtent"),
		FLinearColor(MinXY.X, MinXY.Y, FullExtent.X, FullExtent.Y));

	CachedViewingTeam = Team;
	PaintDebugLogBudget = 5;
	bReady = true;

	World->GetTimerManager().ClearTimer(InitRetryTimer);
	World->GetTimerManager().SetTimer(
		UpdateTimer, this, &USAS_FogOfWarClientComponent::UpdateMask, UpdateIntervalSeconds, true);

	UE_LOG(LogTemp, Log,
		TEXT("USAS_FogOfWarClientComponent ready: ViewingTeam=%d, RT=%dx%d, Bounds=(Center=%s, HalfExtent=%s)."),
		static_cast<int32>(CachedViewingTeam), RTResolution, RTResolution,
		*WorldOrigin.ToString(), *WorldExtent.ToString());
}

void USAS_FogOfWarClientComponent::EnsureRuntimeVirtualTextureVolume(UWorld* World, URuntimeVirtualTexture* FogRVT)
{
	if (!World || !FogRVT)
	{
		UE_LOG(LogTemp, Error,
			TEXT("USAS_FogOfWarClientComponent cannot create/find RVT volume because FogRuntimeVirtualTexture is not set on BP_FogWriter."));
		return;
	}

	for (TActorIterator<ARuntimeVirtualTextureVolume> It(World); It; ++It)
	{
		ARuntimeVirtualTextureVolume* ExistingVolume = *It;
		if (ExistingVolume && ExistingVolume->VirtualTextureComponent
			&& ExistingVolume->VirtualTextureComponent->GetVirtualTexture() == FogRVT)
		{
			const FVector VolumeLocation(
				WorldOrigin.X - WorldExtent.X,
				WorldOrigin.Y - WorldExtent.Y,
				-FogRVTVolumeHeight * 0.5f);
			const FVector VolumeScale(WorldExtent.X * 2.f, WorldExtent.Y * 2.f, FogRVTVolumeHeight);
			ExistingVolume->VirtualTextureComponent->SetMobility(EComponentMobility::Movable);
			ExistingVolume->SetActorLocation(VolumeLocation);
			ExistingVolume->SetActorScale3D(VolumeScale);
			FogVirtualTextureComponent = ExistingVolume->VirtualTextureComponent;

			UE_LOG(LogTemp, Log,
				TEXT("USAS_FogOfWarClientComponent using existing RVT volume '%s' for '%s': Location=%s, Scale=%s."),
				*ExistingVolume->GetName(), *FogRVT->GetName(),
				*ExistingVolume->GetActorLocation().ToString(), *ExistingVolume->GetActorScale3D().ToString());
			return;
		}
	}

	const FVector VolumeLocation(
		WorldOrigin.X - WorldExtent.X,
		WorldOrigin.Y - WorldExtent.Y,
		-FogRVTVolumeHeight * 0.5f);
	const FVector VolumeScale(WorldExtent.X * 2.f, WorldExtent.Y * 2.f, FogRVTVolumeHeight);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Name = MakeUniqueObjectName(World, ARuntimeVirtualTextureVolume::StaticClass(), TEXT("FogOfWar_RVTVolume"));
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	OwnedFogVolume = World->SpawnActor<ARuntimeVirtualTextureVolume>(
		ARuntimeVirtualTextureVolume::StaticClass(), VolumeLocation, FRotator::ZeroRotator, SpawnParams);

	if (!OwnedFogVolume || !OwnedFogVolume->VirtualTextureComponent)
	{
		UE_LOG(LogTemp, Error,
			TEXT("USAS_FogOfWarClientComponent failed to spawn FogOfWar_RVTVolume."));
		return;
	}

	OwnedFogVolume->VirtualTextureComponent->SetMobility(EComponentMobility::Movable);
	OwnedFogVolume->SetActorScale3D(VolumeScale);
	OwnedFogVolume->VirtualTextureComponent->SetVirtualTexture(FogRVT);
	FogVirtualTextureComponent = OwnedFogVolume->VirtualTextureComponent;

	UE_LOG(LogTemp, Log,
		TEXT("USAS_FogOfWarClientComponent spawned RVT volume '%s': Texture=%s, Location=%s, Scale=%s."),
		*OwnedFogVolume->GetName(), *FogRVT->GetName(),
		*OwnedFogVolume->GetActorLocation().ToString(), *OwnedFogVolume->GetActorScale3D().ToString());
}

FBoxSphereBounds USAS_FogOfWarClientComponent::GetFogWorldBounds() const
{
	const FVector Min(
		WorldOrigin.X - WorldExtent.X,
		WorldOrigin.Y - WorldExtent.Y,
		-FogRVTVolumeHeight * 0.5f);
	const FVector Max(
		WorldOrigin.X + WorldExtent.X,
		WorldOrigin.Y + WorldExtent.Y,
		FogRVTVolumeHeight * 0.5f);

	return FBoxSphereBounds(FBox(Min, Max));
}

void USAS_FogOfWarClientComponent::InvalidateFogRuntimeVirtualTexture() const
{
	if (!FogVirtualTextureComponent)
	{
		return;
	}

	FogVirtualTextureComponent->Invalidate(GetFogWorldBounds());
}

void USAS_FogOfWarClientComponent::UpdateMask()
{
	if (!bReady) { return; }
	if (!RT_A || !RT_B) { return; }

	USAS_VisionManagerComponent* Manager = GetVisionManager();
	if (!Manager) { bReady = false; return; }

	const ESAS_Team CurrentTeam = ResolveViewingTeam();
	if (CurrentTeam != CachedViewingTeam)
	{
		NotifyViewingTeamChanged(CurrentTeam);
	}

	UCanvasRenderTarget2D* PrevRT = (ActiveRTIndex == 0) ? RT_A : RT_B;
	UCanvasRenderTarget2D* NextRT = (ActiveRTIndex == 0) ? RT_B : RT_A;

	PaintMaskInto(NextRT, PrevRT, CurrentTeam);

	ActiveRTIndex = 1 - ActiveRTIndex;

	if (OwnedFogWriter)
	{
		OwnedFogWriter->SetMaskTexture(NextRT);
	}
	if (OwnedFogOverlay)
	{
		OwnedFogOverlay->SetMaskTexture(NextRT);
	}
	InvalidateFogRuntimeVirtualTexture();
}

void USAS_FogOfWarClientComponent::PaintMaskInto(
	UCanvasRenderTarget2D* TargetRT, UCanvasRenderTarget2D* PreviousRT, ESAS_Team ViewingTeam)
{
	if (!TargetRT || !PreviousRT) { return; }

	USAS_VisionManagerComponent* Manager = GetVisionManager();
	if (!Manager) { return; }

	UCanvas* Canvas = nullptr;
	FVector2D CanvasSize = FVector2D::ZeroVector;
	FDrawToRenderTargetContext DrawContext;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, TargetRT, Canvas, CanvasSize, DrawContext);
	if (!Canvas)
	{
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, DrawContext);
		return;
	}

	// Pass 1: copy G channel from PreviousRT into TargetRT (clears R, B, A in the process).
	if (CopyGMaterial)
	{
		UMaterialInstanceDynamic* CopyMID = UMaterialInstanceDynamic::Create(CopyGMaterial, this);
		if (CopyMID)
		{
			CopyMID->SetTextureParameterValue(TEXT("SourceRT"), PreviousRT);
			Canvas->K2_DrawMaterial(CopyMID, FVector2D::ZeroVector, CanvasSize, FVector2D::ZeroVector, FVector2D::UnitVector);
		}
	}

	// Pass 2 & 3: for each source on ViewingTeam, paint soft and hard brushes additively.
	const TArray<TWeakObjectPtr<USAS_VisionComponent>>& Sources = Manager->GetSources(ViewingTeam);
	int32 PaintedSourceCount = 0;

	UMaterialInstanceDynamic* SoftMID = UMaterialInstanceDynamic::Create(PaintSoftMaterial, this);
	UMaterialInstanceDynamic* HardMID = UMaterialInstanceDynamic::Create(PaintHardMaterial, this);
	if (SoftMID) { SoftMID->SetTextureParameterValue(TEXT("Brush"), SoftBrushTexture.Get()); }
	if (HardMID) { HardMID->SetTextureParameterValue(TEXT("Brush"), HardBrushTexture.Get()); }

	for (const TWeakObjectPtr<USAS_VisionComponent>& WeakSource : Sources)
	{
		const USAS_VisionComponent* Source = WeakSource.Get();
		if (!Source) { continue; }
		if (!Source->IsSource()) { continue; }

		AActor* SourceOwner = Source->GetOwner();
		if (!SourceOwner) { continue; }

		const FVector WorldLoc = SourceOwner->GetActorLocation();
		const FVector2D UV = WorldToMaskUV(WorldLoc);
		// Allow brushes to bleed slightly past the edge; reject far-off sources.
		if (UV.X < -0.5f || UV.X > 1.5f || UV.Y < -0.5f || UV.Y > 1.5f) { continue; }

		const float RadiusCm = Source->GetCachedRadius();
		if (RadiusCm <= 0.f) { continue; }

		const FVector2D RadiusUV(RadiusCm / (WorldExtent.X * 2.f), RadiusCm / (WorldExtent.Y * 2.f));
		const FVector2D PixelTopLeft(
			(UV.X - RadiusUV.X) * CanvasSize.X,
			(UV.Y - RadiusUV.Y) * CanvasSize.Y);
		const FVector2D PixelSize(
			RadiusUV.X * 2.f * CanvasSize.X,
			RadiusUV.Y * 2.f * CanvasSize.Y);

		if (SoftMID)
		{
			Canvas->K2_DrawMaterial(SoftMID, PixelTopLeft, PixelSize, FVector2D::ZeroVector, FVector2D::UnitVector);
		}
		if (HardMID)
		{
			Canvas->K2_DrawMaterial(HardMID, PixelTopLeft, PixelSize, FVector2D::ZeroVector, FVector2D::UnitVector);
		}
		++PaintedSourceCount;
	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, DrawContext);

	if (PaintDebugLogBudget > 0 || PaintedSourceCount == 0)
	{
		UE_LOG(LogTemp, Display,
			TEXT("USAS_FogOfWarClientComponent paint: Team=%d Sources=%d Painted=%d Target=%s Prev=%s."),
			static_cast<int32>(ViewingTeam), Sources.Num(), PaintedSourceCount,
			*GetNameSafe(TargetRT), *GetNameSafe(PreviousRT));
		if (PaintDebugLogBudget > 0)
		{
			--PaintDebugLogBudget;
		}
	}
}
