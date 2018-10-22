// Copyright 2015-2017 Vagen Ayrapetyan

#include "System/SystemSaveGame.h"
#include "Rade.h"

USystemSaveGame::USystemSaveGame(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	SaveSlotName = TEXT("RadeSaveSlot");
	UserIndex = 0;
}
