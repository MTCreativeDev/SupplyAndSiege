


#include "Core/Actors/SAS_BL_BuildJob.h"
#include "Core/Components/SAS_UnitInformationComponent.h"
#include "Core/Components/SAS_InventoryComponent.h"
#include "Misc/DataAssets/SAS_InventoryProfileData.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"
#include "Misc/DataAssets/SAS_BuildingDefinitionData.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "Components/StaticMeshComponent.h"
#include "Core/Objects/SAS_LogisticsMasterJob.h"
#include "Core/Objects/LMJs/SAS_LMJ_DeliverItem.h"
#include "Misc/Structs/SAS_LogisticsJobWidgetInfo.h"
#include "Kismet/GameplayStatics.h"

ASAS_BL_BuildJob::ASAS_BL_BuildJob()
{
	UnitInformation = CreateDefaultSubobject<USAS_UnitInformationComponent>(TEXT("UnitInformation"));
	Inventory = CreateDefaultSubobject< USAS_InventoryComponent>(TEXT("Inventory"));

}

void ASAS_BL_BuildJob::InitializeBuildJob(ESAS_Team NewAssignedTeam)
{
	UnitInformation->SetTeam(NewAssignedTeam);

	if (!ConstructionInventoryProfile) return;
	Inventory->SetInventoryProfile(ConstructionInventoryProfile);

	if (BuildingUnderConstructionMaterial)
	{
		TArray<UStaticMeshComponent*> Meshes = { PrimaryMesh, SecondaryMesh };

		for (UStaticMeshComponent* Mesh : Meshes)
		{
			if (!Mesh) continue;

			const int32 MaterialCount = Mesh->GetNumMaterials();
			for (int32 i = 0; i < MaterialCount; i++)
			{
				Mesh->SetMaterial(i, BuildingUnderConstructionMaterial);
			}
		}
	}

	RequestResourceDeliveryJobs();

	return;
}

void ASAS_BL_BuildJob::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (USAS_LogisticsMasterJob* Job : ItemDeliveryJobs)
	{
		if (IsValid(Job))
		{
			Job->OnLogisticsMasterJobUpdated.RemoveAll(this);
		}
	}

	ItemDeliveryJobs.Reset();

	Super::EndPlay(EndPlayReason);
}

void ASAS_BL_BuildJob::HandleDeliveryJobUpdated(USAS_LogisticsMasterJob* UpdatedJob)
{
	if (IsActorBeingDestroyed()) return;
	if (!IsValid(UpdatedJob)) return;
	if (!ItemDeliveryJobs.Contains(UpdatedJob)) return;

	UpdateBuildProgress();
}

void ASAS_BL_BuildJob::UpdateBuildProgress()
{
	int32 TotalRequested = 0;
	int32 TotalDelivered = 0;

	for (USAS_LogisticsMasterJob* Job : ItemDeliveryJobs)
	{
		USAS_LMJ_DeliverItem* DeliverJob = Cast<USAS_LMJ_DeliverItem>(Job);
		if (!IsValid(DeliverJob)) continue;

		const FSAS_LogisticsJobWidgetInfo Info = DeliverJob->GetJobInfoForWidget();

		TotalRequested += Info.RequestedAmount;
		TotalDelivered += Info.DeliveredAmount;
	}

	const float Progress = TotalRequested > 0 ? static_cast<float>(TotalDelivered) / static_cast<float>(TotalRequested) : 1.f;

	// TODO: need to create worldspace widget for the progress bar.

	if (Progress >= 1.f)
	{
		//TODO: In the future need to address the actual build job portion that should happen after item delivery is complete.
		CompleteBuildJob();
	}
}

void ASAS_BL_BuildJob::CompleteBuildJob()
{
	if (!BuildingDefinition || !BuildingDefinition->CompletedBuildingClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	ASAS_SelectableBuilding* ConstructedBuilding;
	FTransform ConstructedBuildingTransform = GetActorTransform();

	ConstructedBuilding = World->SpawnActorDeferred<ASAS_SelectableBuilding>(
		BuildingDefinition->CompletedBuildingClass,
		ConstructedBuildingTransform,
		nullptr,
		nullptr	
	);

	ConstructedBuilding->AssignTeamOnSpawn = UnitInformation->AssignedTeam;
	UGameplayStatics::FinishSpawningActor(ConstructedBuilding, ConstructedBuildingTransform);

	for (USAS_LogisticsMasterJob* Job : ItemDeliveryJobs)
	{
		if (IsValid(Job))
		{
			Job->OnLogisticsMasterJobUpdated.RemoveAll(this);
		}
	}

	ItemDeliveryJobs.Reset();
	Destroy();
}

void ASAS_BL_BuildJob::RequestResourceDeliveryJobs()
{
	if (!BuildingDefinition) return;
	if (BuildingDefinition->BuildingCost.ResourceCost.IsEmpty()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GS = World->GetGameState();
	if (!GS) return;

	USAS_LogisticsManagerComponent* LMC = GS->FindComponentByClass<USAS_LogisticsManagerComponent>();
	if (!LMC) return;

	FSAS_ResourceDeliveryRequest DeliveryRequest;

	DeliveryRequest.BuildSite = this;
	DeliveryRequest.ResourceCost = BuildingDefinition->BuildingCost.ResourceCost;
	DeliveryRequest.Priority = 1;
		//TODO: Implement priority

	ItemDeliveryJobs = LMC->CreateBuildSiteResourceDeliveryJob(DeliveryRequest);

	for (USAS_LogisticsMasterJob* Job : ItemDeliveryJobs)
	{
		if (!IsValid(Job)) continue;
		Job->OnLogisticsMasterJobUpdated.AddUObject(this, &ASAS_BL_BuildJob::HandleDeliveryJobUpdated);
	}
}
