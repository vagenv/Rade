// Copyright 2015-2021 Vagen Ayrapetyan

#pragma once

#include "GameFramework/GameState.h"
#include "../RadeData.h"
#include "RadeGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleEventDelegate);

// Custom Game State
UCLASS()
class RADE_API ARadeGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	ARadeGameState();
	virtual void BeginPlay();


	// Array of All Online Messages
	UPROPERTY(ReplicatedUsing = OnRep_MessagesList, EditAnywhere, BlueprintReadOnly, Category = " ")
		TArray<FRadeOnineMessageData> TheMessages;

	// Events when Message List Replicated
	UFUNCTION()
		void OnRep_MessagesList();

	// Chat Update Delegate
	UPROPERTY(BlueprintAssignable, Category = "")
		FSimpleEventDelegate ChatUpdateDelegate;

	//Add Chat Message, Should Be called on Server
	void AddNewChatMessage(FString TheMessage, class ARadePlayer* ThePlayer);
};
