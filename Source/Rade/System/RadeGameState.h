// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/GameState.h"
#include "RadeData.h"
#include "RadeGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleEventDelegate);

UCLASS()
class RADE_API ARadeGameState : public AGameState
{
	GENERATED_BODY()
	

public:


	// Constructor
	ARadeGameState();
	virtual void BeginPlay();


	UPROPERTY(ReplicatedUsing = OnRep_MessagesList, EditAnywhere, BlueprintReadOnly, Category = " ")
		TArray<FRadeOnineMessageData> TheMessages;

	UFUNCTION()
		void OnRep_MessagesList();

	UPROPERTY(BlueprintAssignable, Category = "")
		FSimpleEventDelegate ChatUpdateDelegate;

	void AddNewChatMessage(FString TheMessage, class ARadePlayer* ThePlayer);




	void UpdatePlayerStats();


};
