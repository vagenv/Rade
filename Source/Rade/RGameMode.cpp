// Copyright Epic Games, Inc. All Rights Reserved.

#include "RadeGameMode.h"
#include "RadeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARadeGameMode::ARadeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
