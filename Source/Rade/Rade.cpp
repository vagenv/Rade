// Copyright 2015-2016 Vagen Ayrapetyan

#include "Rade.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Rade, "Rade" );

DEFINE_LOG_CATEGORY(RadeLog);

void rlog (FString txt) 
{
   UE_LOG(RadeLog, Warning, TEXT("%s"), *txt);
} 