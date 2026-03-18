

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_BuildingLayout.h"
#include "SAS_BL_DefinitionAuthoring.generated.h"

/**
 * 
 */
UCLASS()
class SUPPLYANDSIEGE_API ASAS_BL_DefinitionAuthoring : public ASAS_BuildingLayout
{
	GENERATED_BODY()
	
public:
	ASAS_BL_DefinitionAuthoring();


protected:
	UFUNCTION(CallInEditor, Category = "SAS_Building")
	void LoadFromDefinition();

	UFUNCTION(CallInEditor, Category = "SAS_Building")
	void ApplyToDefinition();

	void CaptureMoveToLocationsToDefinition();

};
