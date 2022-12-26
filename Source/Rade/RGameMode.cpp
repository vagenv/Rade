// Copyright Epic Games, Inc. All Rights Reserved.

#include "RGameMode.h"
#include "RUtilLib/Rlog.h"
// #include "UObject/ConstructorHelpers.h"

ARGameMode::ARGameMode()
{
   // // set default pawn class to our Blueprinted character
   // static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
   // if (PlayerPawnBPClass.Class != NULL)
   // {
   //    DefaultPawnClass = PlayerPawnBPClass.Class;
   // }
}

void ARGameMode::BeginPlay ()
{
   Super::BeginPlay ();
   R_LOG ("Game mode started");
}

