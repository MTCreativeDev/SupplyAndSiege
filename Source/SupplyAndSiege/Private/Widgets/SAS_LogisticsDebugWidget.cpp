


#include "Widgets/SAS_LogisticsDebugWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Core/Components/SAS_LogisticsManagerComponent.h"

void USAS_LogisticsDebugWidget::NativeConstruct()
{

	UWorld* World = GetWorld();
	if (!World)
	{
		Super::NativeConstruct();
		return;
	}

	AGameStateBase* GS = World->GetGameState();
	LogisticsManagerComponent = GS->FindComponentByClass< USAS_LogisticsManagerComponent>();

	Super::NativeConstruct();
}
