


#include "Core/Components/SAS_UnitControlComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"


USAS_UnitControlComponent::USAS_UnitControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

ESAS_CommandHandlingResult USAS_UnitControlComponent::HandleGroundRightClick(const FVector& WorldLocation)
{
	return ESAS_CommandHandlingResult::Ignored;
}

ESAS_CommandHandlingResult USAS_UnitControlComponent::HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	return ESAS_CommandHandlingResult::Ignored;
}

ESAS_CommandHandlingResult USAS_UnitControlComponent::HandleActorRightClick(AActor* TargetActor, const FVector& ClickLocation)
{
	return ESAS_CommandHandlingResult::Ignored;
}

bool USAS_UnitControlComponent::CanReceivePlayerCommands() const
{
	return UnitInformation != nullptr;
}



void USAS_UnitControlComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		UnitInformation = Owner->FindComponentByClass<USAS_UnitInformationComponent>();
	}	
}

