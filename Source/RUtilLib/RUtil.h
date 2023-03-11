

#pragma once

#include "Engine.h"
#include "RUtil.generated.h"

class UActorComponent;
class ACharacter;

class RUTILLIB_API URUtil
{
public:
   template<typename T>
   static T* GetComponent (const AActor * Target);
};

template<typename T>
T* URUtil::GetComponent (const AActor * Target)
{
   if (!ensure (Target)) return nullptr;
   T* CompObj = nullptr;
   {
      TArray<T*> CompObjList;
      Target->GetComponents (CompObjList);
      if (CompObjList.Num ()) CompObj = CompObjList[0];
   }
   if (!CompObj) return nullptr;
   return CompObj;
}

UCLASS()
class RUTILLIB_API URUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // Get local player for HUD
   UFUNCTION(BlueprintPure, Category = "Rade|Util", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Local Rade Player", CompactNodeTitle = "Get Rade Player", Keywords = "get rade player"))
      static ACharacter* GetLocalRadePlayer (UObject* WorldContextObject);
};



