


#include "Core/Components/SAS_UnitControlComponent.h"
#include "Core/Components/SAS_UnitInformationComponent.h"


USAS_UnitControlComponent::USAS_UnitControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool USAS_UnitControlComponent::GroundSelectIsMove() const
{
	return true;
}

void USAS_UnitControlComponent::HandleGroundRightClick(const FVector& WorldLocation)
{
	return;
}

bool USAS_UnitControlComponent::ResourceSelectIsMove(USAS_ResourceTypeData* ResourceType) const
{
	return false;
}

bool USAS_UnitControlComponent::CanReceiveAttackOrder() const
{
	return false;
}

void USAS_UnitControlComponent::HandleResourceRightClick(USAS_ResourceTypeData* ResourceType, FSAS_ResourceKey ResourceKey, const FVector& ResourceLocation)
{
	return;
}

void USAS_UnitControlComponent::HandleAttackUnitOrder(USAS_UnitInformationComponent* TargetUnitInformationComponent, const FVector& ClickLocation)
{
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

