


#include "Core/Components/SAS_TimeManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"

USAS_TimeManagerComponent::USAS_TimeManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = TickInterval;
}


void USAS_TimeManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.TickInterval = TickInterval;

	DirectionalLight = Cast<ADirectionalLight>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass())
	);

	if (!IsValid(DirectionalLight)) return;

	CurrentSunPitch = StartingPitch;

	const float TicksPerDay = DayLengthInSeconds / TickInterval;
	PitchIncrementPerTick = (MaxPitch - StartingPitch) / TicksPerDay;

	DirectionalLight->SetActorRotation(FRotator(CurrentSunPitch, 0.f, 0.f));
}

void USAS_TimeManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ProgressTime();
}

void USAS_TimeManagerComponent::ProgressTime()
{
	if (!IsValid(DirectionalLight)) return;

	CurrentSunPitch += PitchIncrementPerTick;

	if (CurrentSunPitch >= MaxPitch)
	{
		CurrentSunPitch = StartingPitch;
	}

	DirectionalLight->SetActorRotation(FRotator(CurrentSunPitch, 0.f, 0.f));
}

