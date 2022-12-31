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
   GENERATED_USTRUCT_BODY()
public:

   // Date of Message
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
      FDateTime Time;

   // The Actual Message
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
      FString Message = "Undefined";

   // Player Name of the Message Owner
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
      FString Owner = "Undefined";

   // Message Color
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "")
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
   UPROPERTY(ReplicatedUsing = OnRep_MessagesList, EditAnywhere, BlueprintReadOnly, Category = "RadeSession")
      TArray<FROnineMessageData> Messages;

   // Events when Message List Replicated
   UFUNCTION()
      void OnRep_MessagesList();

   // Chat Update Delegate
   UPROPERTY(BlueprintAssignable, Category = "RadeSession")
      FSimpleEventDelegate ChatUpdateDelegate;

   //Add Chat Message, Should Be called on Server
   void AddChatMessage (FString Message_, FString owner_, FColor color_);
};

