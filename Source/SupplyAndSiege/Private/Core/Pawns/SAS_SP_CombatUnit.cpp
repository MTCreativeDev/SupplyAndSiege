


#include "Core/Pawns/SAS_SP_CombatUnit.h"
#include "Core/Components/SAS_CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"

ASAS_SP_CombatUnit::ASAS_SP_CombatUnit()
{
	CombatComponent = CreateDefaultSubobject<USAS_CombatComponent>(TEXT("CombatComponent"));
}


