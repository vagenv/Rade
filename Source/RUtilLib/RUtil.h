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

   // Get world object only if it is not being destroyed
   static UWorld* GetWorld (const UObject* WorldContextObject);

   // Check that the world object is not null
   #define R_IS_VALID_WORLD (URUtil::GetWorld (this) != nullptr)

   // Get pawn of the first player controller
   static ACharacter* GetLocalRadePlayer (const UObject* WorldContextObject);

   // Uses Eval call of FRuntimeFloatCurve as const object
   static float GetRuntimeFloatCurveValue (const FRuntimeFloatCurve& InCurve, float InTime);

   // Gets angle in degrees between two vectors
   static float GetAngle (FVector v1, FVector v2);

   // Gets path to UDataTable object
   static FString GetTablePath (const UDataTable* Table);

   // Compare if array contain same objects
   static bool ObjectArray_EqualEqual (const TArray<UObject*> &A,
                                       const TArray<UObject*> &B);

   // --- Template functions

   // Get specific component of target actor
   template<typename T>
   static FORCEINLINE T* GetComponent (const AActor* Target);

   // Create actor component from a class object, attach to actor and return the created component
   template<typename T>
   static FORCEINLINE T* AddComponent (AActor* Target, const TSubclassOf<UActorComponent> CompClass);

   // Get instance of RWorld*Mgr which should be component of GameState
   template<typename T>
   static FORCEINLINE T* GetWorldInstance (const UObject* WorldContextObject);
};

// Get specific component of target actor
template<typename T>
T* URUtil::GetComponent (const AActor* Target)
{
   if (!ensure (Target)) return nullptr;
   T* ResultObj = Target->FindComponentByClass<T>();
   if (!IsValid (ResultObj)) return nullptr;
   return ResultObj;
}

// Create actor component from a class object, attach to actor and return the created component
template<typename T>
T* URUtil::AddComponent (AActor* Target, const TSubclassOf<UActorComponent> CompClass)
{
   if (!ensure (Target)) return nullptr;
   if (!ensure (CompClass)) return nullptr;

   UActorComponent *NewComp = Target->AddComponentByClass (CompClass, false, FTransform(), false);
   if (!ensure (NewComp)) return nullptr;

   T *TargetComp = Cast<T>(NewComp);
   ensure (NewComp);
   return TargetComp;
}

// Get instance of RWorld*Mgr which should be component of GameState
template<typename T>
T* URUtil::GetWorldInstance (const UObject* WorldContextObject)
{
   if (!ensure (WorldContextObject)) return nullptr;

   UWorld *World = URUtil::GetWorld (WorldContextObject);
   if (!World) return nullptr;

   // All world instances should be kept in game state.
   return URUtil::GetComponent<T> (World->GetGameState ());
}


// Blueprint interface to URUtil class
UCLASS()
class RUTILLIB_API URUtilLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:

   // Get pawn of the first player controller
   UFUNCTION(BlueprintPure, Category = "Rade|Util", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Local Rade Player", CompactNodeTitle = "Get Rade Player", Keywords = "get rade player"))
      static ACharacter* GetLocalRadePlayer (const UObject* WorldContextObject) {
         return URUtil::GetLocalRadePlayer (WorldContextObject);
      }

   // Uses Eval call of FRuntimeFloatCurve as const object
   UFUNCTION(BlueprintPure, Category = "Rade|Util")
      static float GetRuntimeFloatCurveValue (const FRuntimeFloatCurve& InCurve, float InTime) {
         return URUtil::GetRuntimeFloatCurveValue (InCurve, InTime);
      }


   // Gets angle in degrees between two vectors
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Util")
      static float GetAngle (FVector v1, FVector v2) {
         return URUtil::GetAngle (v1, v2);
      }

   // Gets path to UDataTable object
   UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade|Util")
      static FString GetTablePath (const UDataTable* Table) {
         return URUtil::GetTablePath (Table);
      }

   UFUNCTION(BlueprintPure, Category = "Rade|Util", meta=(DisplayName="==", CompactNodeTitle="=="))
      static bool ObjectArray_EqualEqual (const TArray<UObject*> &A,
                                          const TArray<UObject*> &B) {
         return URUtil::ObjectArray_EqualEqual (A, B);
      }
};

