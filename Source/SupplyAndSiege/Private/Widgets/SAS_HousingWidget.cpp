


#include "Widgets/SAS_HousingWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_PopulationManagerComponent.h"

void USAS_HousingWidget::NativeConstruct()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		Super::NativeConstruct();
		return;
	}

	AGameStateBase* GS = World->GetGameState();
	PopulationManagerComponent = GS->FindComponentByClass< USAS_PopulationManagerComponent>();

	Super::NativeConstruct();
}
