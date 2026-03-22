


#include "Core/Actors/SAS_BL_BuildPlacement.h"
#include "Components/BoxComponent.h"



void ASAS_BL_BuildPlacement::BeginPlay()
{
	CheckPlacementValidity();
	UpdateMeshMaterials();
}

void ASAS_BL_BuildPlacement::ApplyDefinitionToComponents()
{
	Super::ApplyDefinitionToComponents();

	
}

void ASAS_BL_BuildPlacement::CheckPlacementValidity()
{
	bool NewValidity = false;
	if (!BuildingAreaCollision)
	{
		if (NewValidity != bIsValidPlacement)
		{
			bIsValidPlacement = NewValidity;
			UpdateMeshMaterials();
		}
		else
		{
			bIsValidPlacement = NewValidity;
		}

		return;
	}

	BuildingAreaCollision->UpdateOverlaps();

	TArray<AActor*> OverlappingActors;
	BuildingAreaCollision->GetOverlappingActors(OverlappingActors);

	NewValidity = (OverlappingActors.Num() == 0);
	if (bIsValidPlacement != NewValidity)
	{
		bIsValidPlacement = NewValidity;
		UpdateMeshMaterials();

	}
	else
	{
		bIsValidPlacement = NewValidity;
	}
}

void ASAS_BL_BuildPlacement::UpdateMeshMaterials()
{
	if (!ValidPlacementMaterial || !InvalidPlacementMaterial) return;

	TObjectPtr<UMaterialInterface> MaterialToSet;

	if (bIsValidPlacement)
	{
		MaterialToSet = ValidPlacementMaterial;
	}
	else
	{
		MaterialToSet = InvalidPlacementMaterial;
	}

	if (PrimaryMesh)
	{
		int32 NumMats = PrimaryMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMats; i++)
		{
			PrimaryMesh->SetMaterial(i, MaterialToSet);
		}
	}
	if (SecondaryMesh)
	{
		int32 NumMats = SecondaryMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMats; i++)
		{
			SecondaryMesh->SetMaterial(i, MaterialToSet);
		}
	}

}


