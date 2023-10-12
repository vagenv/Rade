// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "Engine.h"
#include "RUtil.generated.h"

class UActorComponent;
class ACharacter;
class UWorld;


class RUTILLIB_API URUtil
{
public:

   static UWorld* GetWorld (const UObject* WorldContextObject);
   #define R_IS_VALID_WORLD (URUtil::GetWorld (this) != nullptr)


   static ACharacter* GetLocalRadePlayer (UObject* WorldContextObject);
   
   static float GetRuntimeFloatCurveValue (const FRuntimeFloatCurve& InCurve, float InTime);

   static float GetAngle (FVector v1, FVector v2);

   static FString GetTablePath (const UDataTable* Table);



   // Template functions

   template<typename T>
   static FORCEINLINE T* GetComponent (const AActor* Target);

   template<typename T>
   static FORCEINLINE T* AddComponent (AActor* Target, const TSubclassOf<UActorComponent> CompClass);

   template<typename T>
   static FORCEINLINE T* GetWorldInstance (const UObject* WorldContextObject);

   template <typename T>
   static FORCEINLINE T* LoadObjFromPath (const FString& Path);
};

template<typename T>
T* URUtil::GetComponent (const AActor* Target)
{
   if (!ensure (Target)) return nullptr;
   T* ResultObj = Target->FindComponentByClass<T>();
   if (!IsValid (ResultObj))       return nullptr;
   return ResultObj;
}

template<typename T>
T* URUtil::AddComponent (AActor* Target, const TSubclassOf<UActorComponent> CompClass)
{
   if (!ensure (Target))    return nullptr;
   if (!ensure (CompClass)) return nullptr;

   UActorComponent *NewComp = Target->AddComponentByClass (CompClass, false, FTransform(), false);
   if (!ensure (NewComp)) return nullptr;

   T *TargetComp = Cast<T>(NewComp);
   ensure (NewComp);
   return TargetComp;
}

template<typename T>
T* URUtil::GetWorldInstance (const UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = URUtil::GetWorld (WorldContextObject);
   if (!World) return nullptr;

   // All world instances should be kept in game state.
   return URUtil::GetComponent<T> (World->GetGameState ());
}

template<typename T>
T* URUtil::LoadObjFromPath (const FString& Path)
{
   if (Path.IsEmpty ()) return nullptr;

   // TODO. Add checks that path is valid and file exists.

   return Cast<T> (StaticLoadObject (T::StaticClass (), NULL, *Path));
}

UCLASS()
class RUTILLIB_API URUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // Get local player
   UFUNCTION(BlueprintPure, Category = "Rade|Util", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Local Rade Player", CompactNodeTitle = "Get Rade Player", Keywords = "get rade player"))
      static ACharacter* GetLocalRadePlayer (UObject* WorldContextObject) {
         return URUtil::GetLocalRadePlayer (WorldContextObject);
      }

   // Uses Eval call of FRuntimeFloatCurve
   UFUNCTION(BlueprintPure, Category = "Rade|Util")
	   static float GetRuntimeFloatCurveValue (const FRuntimeFloatCurve& InCurve, float InTime) {
         return URUtil::GetRuntimeFloatCurveValue (InCurve, InTime);
      }


   // Gets angle in degrees between two vectors
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Util")
      static float GetAngle (FVector v1, FVector v2) {
         return URUtil::GetAngle (v1, v2);
      }

   // Gets path to table
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Util")
      static FString GetTablePath (const UDataTable* Table) {
         return URUtil::GetTablePath (Table);
      }
};

