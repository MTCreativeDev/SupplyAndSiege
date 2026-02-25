// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Controllers/SAS_AI_Controller.h"

#include "Components/StateTreeAIComponent.h"

ASAS_AI_Controller::ASAS_AI_Controller() 
{
	StateTreeComp = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeComp"));
}
