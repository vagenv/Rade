// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "Animation/AnimInstance.h"
#include "RadeData.h"
#include "RadeAnimInstance.generated.h"



UCLASS()
class RADE_API URadeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// Default Events
	URadeAnimInstance(const class FObjectInitializer& PCIP);

	void BeginPlay();


	// Main Properties

	// Current Weapon Archtype
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		EAnimArchetype AnimArchetype;

	// Current Animation playing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Anim State")
		EAnimState PlayerCurrentAnimState;

	// Reference to the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class ARadePlayer* ThePlayer;

	// Notification in blueprint that animation started
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Anim State")
		void BP_AnimStarted(EAnimState currentAnimCheck);

	// Set new Animation
	void PlayLocalAnim(EAnimState currentAnimCheck);

	// Recieve the anim from player Class
	void RecieveGlobalAnimID(EAnimState currentAnimCheck);


	/// Global Anim state Check
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsAnimState(EAnimState currentAnimCheck);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsAnimArchetype(EAnimArchetype AnimArchtypeCheck);

	// Check value from player class
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool CanFireInAir();

	// Event called in blueprint when Jump_Start -> Jump_Idle 
	UFUNCTION(BlueprintCallable, Category = "Player Anim State")
		void InAirIdleStateEntered();


	// In air check from current anim state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsInAir();

	
};
