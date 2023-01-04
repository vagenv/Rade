// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "GameFramework/GameState.h"
#include "RGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleEventDelegate);

//=============================================================================
//                     Custom Online Data Type
//=============================================================================

//  Online Message Data
USTRUCT(BlueprintType)
struct FROnineMessageData
{
   GENERATED_BODY()

   // Date of Message
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FDateTime Time;

   // The Actual Message
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Message = "Undefined";

   // Player Name of the Message Owner
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FString Owner = "Undefined";

   // Message Color
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FLinearColor Color = FColor::Magenta;

   FROnineMessageData();
   FROnineMessageData(FString Message_, FString Owner_, FColor Color_);
};

// Custom Game State
UCLASS(Blueprintable)
class RSESSIONMANAGERLIB_API ARGameState : public AGameState
{
   GENERATED_BODY()

public:

   ARGameState();
   virtual void BeginPlay();

   // Array of All Online Messages
   UPROPERTY(ReplicatedUsing = OnRep_MessagesList, EditAnywhere, BlueprintReadOnly, Category = "Rade|Network")
      TArray<FROnineMessageData> Messages;

   // Events when Message List Replicated
   UFUNCTION()
      void OnRep_MessagesList();

   // Chat Update Delegate
   UPROPERTY(BlueprintAssignable, Category = "Rade|Network")
      FSimpleEventDelegate ChatUpdateDelegate;

   //Add Chat Message, Should Be called on Server
   void AddChatMessage (FString Message_, FString owner_, FColor color_);
};

