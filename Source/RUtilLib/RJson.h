#pragma once

#include "Json.h"
#include "JsonObjectConverter.h"

class RUTILLIB_API RJSON
{
public:
   template<typename T>
   static bool ToString (const T &Src, FString &Dst);


   template<typename T>
   static bool ToStruct (const FString &Src, T &Dst);
};

template<typename T>
bool RJSON::ToString (const T &Src, FString &Dst)
{
   return FJsonObjectConverter::UStructToJsonObjectString<T> (Src, Dst, 0, 0, 3);
}

template<typename T>
bool RJSON::ToStruct (const FString &Src, T &Dst)
{
   return FJsonObjectConverter::JsonObjectStringToUStruct (Src, &Dst, 0, 0, true);
}
