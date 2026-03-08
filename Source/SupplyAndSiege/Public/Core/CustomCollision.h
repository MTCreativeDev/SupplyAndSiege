// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//Trace
#define Trace_SelectableUnit ECC_GameTraceChannel1
#define Trace_NavigableArea ECC_GameTraceChannel2
#define Trace_Interactable ECC_GameTraceChannel3

//Objects
#define Object_BuildingBounds ECC_GameTraceChannel4;


namespace SAS_CollisionProfiles
{
	static const FName InventoryCheck(TEXT("InventoryCheck"));
	static const FName BuildingBounds(TEXT("BuildingBounds"));
}