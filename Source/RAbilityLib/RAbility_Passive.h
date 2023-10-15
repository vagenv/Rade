// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RAbility.h"
#include "RAbility_Passive.generated.h"

//=============================================================================
//                 Passive Ability
//=============================================================================

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RABILITYLIB_API URAbility_Passive : public URAbility
{
   GENERATED_BODY()
public:

   URAbility_Passive ();
};

