// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine.h"
#include "RMacro.h"

RUTILLIB_API DECLARE_LOG_CATEGORY_EXTERN(RadeLog, Log, All);

//=============================================================================
//                        Custom Print Methods
//=============================================================================

void RUTILLIB_API __rlog_raw   (const FString &msg);
void RUTILLIB_API __rprint_raw (const FString &msg);

// --- General purpose message output.
void RUTILLIB_API __rlog_internal (const UObject* WorldContext,
                                   const FString &msg,
                                   const char    *file,
                                   int            line,
                                   const char    *func);
#define R_LOG(msg) __rlog_internal (this, FString (msg), __FILENAME__, __LINE__, __FUNCTION__);

void RUTILLIB_API __rprint_internal (const UObject* WorldContext,
                                     const FString &msg,
                                     const char    *file,
                                     int            line,
                                     const char    *func);
#define R_PRINT(msg) __rprint_internal (this, FString (msg), __FILENAME__, __LINE__, __FUNCTION__);

//=============================================================================
//                        Custom Load Object from Path
//=============================================================================

/*
//TEMPLATE Load Obj From Path
template <typename ObjClass>
static ObjClass* LoadObjFromPath(const FName& Path)
{
   if (Path == NAME_None) return NULL;
   return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}
*/

