// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemTypes.h"
#include "Json.h"
#include "JsonObjectConverter.h"

FRItemProperty::FRItemProperty (const FString &Key_, const FString &Value_)
   : Key(Key_), Value(Value_)
{
}

FString FRItemData::ToJSON ()
{
   //ToRAW ();
   FString OutString;
   FJsonObjectConverter::UStructToJsonObjectString<FRItemData> (*this, OutString);
   return OutString;
}

bool FRItemData::FromJSON (const FString &Data_)
{
   bool res = FJsonObjectConverter::JsonObjectStringToUStruct (Data_, this, 0, 0);
   //if (res) FromRAW ();
   return res;
}


//=============================================================================
//                  RAW DATA Util functions
//=============================================================================


bool FRItemData::Has (const FString &Key_) const
{
   for (const FRItemProperty &prop : RawData) {
      if (prop.Key == Key_) return true;
   }
   return false;
}

FString FRItemData::Get (const FString &Key_) const
{
   for (const FRItemProperty &prop : RawData) {
      if (prop.Key == Key_) return prop.Value;
   }
   return "";
}

void FRItemData::Set (const FString &Key_, const FString &Value_)
{
  for (FRItemProperty &prop : RawData) {
      if (prop.Key == Key_) {
         prop.Value = Value_;
         return;
      }
   }
   RawData.Add (FRItemProperty(Key_, Value_));
}



/*

// --- Base version

FString FItemData::Get (const FString &key) const {

   if (RawData.Contains(key)) return RawData[key];
   else                       return "";
}

void FItemData::Set (const FString &key, const FString &value) {
   RawData.Add (key,value);
}


// --- Advanced version

int32 FItemData::GetINT32 (const FString &key) const {
   int32 res = 0;
   if (RawData.Contains(key)) res = FCString::Atoi(*(RawData[key]));
   return res;
}

int64 FItemData::GetINT64 (const FString &key) const {
   int64 res = 0ULL;
   if (RawData.Contains(key)) res = FCString::Atoi64(*(RawData[key]));
   return res;
}

/*
void* FItemData::GetPTR (const FString &key) const {
   void* res = nullptr;
   if (RawData.Contains (key)) {
      FString value = RawData[key];
      if (value != "") res = (void*)FCString::Atoi64(*value);
   }
   return res;
}
*/

/*
float FItemData::GetFloat (const FString &key) const {
   float res = 0.;
   if (RawData.Contains(key)) res = FCString::Atof (*(RawData[key]));
   return res;
}


void FItemData::Set (const FString &key, int32 value) {
   RawData.Add (key, FString::FromInt (value));
}

void FItemData::Set (const FString &key, const int64 value) {
   RawData.Add (key, FString::Printf (TEXT ("%lld"), value));
}

/*
void FItemData::Set (const FString &key, const void* value) {
   if (value == nullptr) RawData.Add (key, "");
   else                  RawData.Add (key, FString::Printf (TEXT ("%llu"), value));
}
*/

/*

void FItemData::Set (const FString &key, const float value) {
   RawData.Add (key, FString::SanitizeFloat (value));
}

// Set all local params from RawData
void FItemData::FromRAW ()
{
   Description.Name     = Get      ("Description.Name");
   //Description.Icon   = Get      ("Description.Icon");
   Description.Count    = GetINT32 ("Description.Count");
   Description.MaxCount = GetINT32 ("Description.MaxCount");
   Description.Weight   = GetFloat ("Description.Weight");
   Description.Tooltip  = Get      ("Description.Tooltip");
}

// Update RawData from local params
void FItemData::ToRAW ()
{
   Set ("Description.Name",     Description.Name);
   //Set ("Description.Count",  Description.Icon);
   Set ("Description.Count",    Description.Count);
   Set ("Description.MaxCount", Description.MaxCount);
   Set ("Description.Weight",   Description.Weight);
   Set ("Description.Tooltip",  Description.Tooltip);
}
*/

