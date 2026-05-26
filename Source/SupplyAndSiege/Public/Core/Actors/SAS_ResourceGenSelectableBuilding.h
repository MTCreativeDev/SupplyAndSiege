#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_SelectableBuilding.h"
#include "SAS_ResourceGenSelectableBuilding.generated.h"


UCLASS()
class SUPPLYANDSIEGE_API ASAS_ResourceGenSelectableBuilding : public ASAS_SelectableBuilding
{
	GENERATED_BODY()

	//Note: No new properties or functions for now, this class just serves as a way to differentiate between buildings 
	// that can generate resources and those that can't, which is useful for the UI and other systems that need to know this distinction.

};