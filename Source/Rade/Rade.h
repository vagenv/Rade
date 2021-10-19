// Copyright 2015-2021 Vagen Ayrapetyan

#pragma once

#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Online.h"
#include "OnlineSubsystem.h"

#define COLLISION_PROJECTILE    ECC_GameTraceChannel1

/// Custom Log Type

DECLARE_LOG_CATEGORY_EXTERN(RadeLog, Log, All);


///////////////////////////////////////////////////////////////////////////////

//								Custom Print Methods

#define printw(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,text)
#define printr(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,  text)
#define printg(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green,text)
#define printb(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, text)

#define printlong(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Red,text)


void rlog (FString txt);


///////////////////////////////////////////////////////////////////////////////

//								Custom Load Object from Path

// Get Path To Object
static FORCEINLINE FName GetObjPath(const UObject* Obj)
{
	if (!Obj) return NAME_None;
	if (!Obj->IsValidLowLevel()) return NAME_None;
	//~

	FSoftObjectPath ThePath (Obj);

	if (!ThePath.IsValid()) return NAME_None;

	//The Class FString Name For This Object
	FString Str = Obj->GetClass()->GetDescription()
					+ "'" + ThePath.ToString() + "'";
	return FName(*Str);
}

//TEMPLATE Load Obj From Path
template <typename ObjClass>
static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;

	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}