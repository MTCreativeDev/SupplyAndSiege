// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SAS_SelectablePawn.generated.h"

class USAS_UnitInformationComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_SelectablePawn : public APawn
{
	GENERATED_BODY()

public:
	ASAS_SelectablePawn();



protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAS_Unit")
	USAS_UnitInformationComponent* UnitInformationComponent;

};
