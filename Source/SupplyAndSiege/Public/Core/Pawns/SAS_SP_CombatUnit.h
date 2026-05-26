

#pragma once

#include "CoreMinimal.h"
#include "Core/Pawns/SAS_SelectablePawn.h"
#include "Core/Interfaces/SAS_PlayerVisibilityInterface.h"
#include "SAS_SP_CombatUnit.generated.h"

class USAS_CombatComponent;

UCLASS()
class SUPPLYANDSIEGE_API ASAS_SP_CombatUnit : public ASAS_SelectablePawn
{
	GENERATED_BODY()
	

public:

	ASAS_SP_CombatUnit();



protected:

	USAS_CombatComponent* CombatComponent;



private:

};
