

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/Structs/SAS_ResourceDeliveryRequest.h"
#include "SAS_LogisticsManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SUPPLYANDSIEGE_API USAS_LogisticsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAS_LogisticsManagerComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void CreateBuildSiteResouceDeliveryJob(FSAS_ResourceDeliveryRequest ResourceDeliveryInformation);
		
};
