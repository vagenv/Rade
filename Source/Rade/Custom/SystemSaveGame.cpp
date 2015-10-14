// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "Custom/SystemSaveGame.h"


USystemSaveGame::USystemSaveGame(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	SaveSlotName = TEXT("RadeSaveSlot");
	UserIndex = 0;
}

