


#include "Core/Components/SAS_IslandManagerComponent.h"

TArray<TWeakObjectPtr<USAS_IslandManagerComponent>> USAS_IslandManagerComponent::GUIDAssignmentQueue;

// Sets default values for this component's properties
USAS_IslandManagerComponent::USAS_IslandManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USAS_IslandManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	GUIDAssignmentQueue.Add(TWeakObjectPtr<USAS_IslandManagerComponent>(this));
}


// Called every frame
void USAS_IslandManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USAS_IslandManagerComponent::CleanupGUIDAssignmentQueue()
{
	GUIDAssignmentQueue.RemoveAll([](const TWeakObjectPtr<USAS_IslandManagerComponent>& Ptr)
		{
			return !Ptr.IsValid();
		});
}

