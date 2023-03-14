// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine.h"
#include "RMacro.h"

RUTILLIB_API DECLARE_LOG_CATEGORY_EXTERN (RadeLog, Log, All);

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
#define R_LOG(msg)        __rlog_internal (this,    FString (msg), __FILENAME__, __LINE__, __FUNCTION__)
#define R_LOG_STATIC(msg) __rlog_internal (nullptr, FString (msg), __FILENAME__, __LINE__, __FUNCTION__)

#define R_LOG_PRINTF(msg, ...)        R_LOG(FString::Printf (TEXT(msg), __VA_ARGS__))
#define R_LOG_STATIC_PRINTF(msg, ...) R_LOG_STATIC(FString::Printf (TEXT(msg), __VA_ARGS__))

void RUTILLIB_API __rprint_internal (const UObject* WorldContext,
                                     const FString &msg,
                                     const char    *file,
                                     int            line,
                                     const char    *func);
#define R_PRINT(msg)        __rprint_internal (this,    FString (msg), __FILENAME__, __LINE__, __FUNCTION__)
#define R_PRINT_STATIC(msg) __rprint_internal (nullptr, FString (msg), __FILENAME__, __LINE__, __FUNCTION__)

