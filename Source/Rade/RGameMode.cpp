// Copyright Epic Games, Inc. All Rights Reserved.

#include "RGameMode.h"
#include "RSaveLib/RSaveMgr.h"

ARGameMode::ARGameMode()
{
   SaveMgr = CreateDefaultSubobject<URSaveMgr>(TEXT("SaveMgr"));
}

void ARGameMode::BeginPlay ()
{
   Super::BeginPlay ();
}

