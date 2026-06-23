

#pragma once

#include "CoreMinimal.h"
#include "Core/Actors/SAS_SB_ResourceDepot.h"
#include "SAS_SB_RD_House.generated.h"

class USAS_PopulationManagerComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_SB_RD_House : public ASAS_SB_ResourceDepot
{
	GENERATED_BODY()

public:
	ASAS_SB_RD_House();

protected:
	virtual void BeginPlay() override;

private:
	void GetPopulationManager();


protected:
	USAS_PopulationManagerComponent* PopulationManagerComponent;

	
};
