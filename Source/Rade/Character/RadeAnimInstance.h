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

	URadeAnimInstance(const class FObjectInitializer& PCIP);

	void BeginPlay();


	// Find/Reset Reference to The Character Class
	void ResetRadeCharacterRef();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//										Main References and Properties


	// Current Weapon Archtype
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade")
		EAnimArchetype AnimArchetype;

	// Current Animation playing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rade" )
		EAnimState PlayerCurrentAnimState;

	// Reference to the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class ARadeCharacter* TheCharacter;



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//										Animation Control and Events


	// Notification in blueprint that animation started
		UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_AnimStarted(EAnimState currentAnimCheck);


	// Set Animation Archetype
		UFUNCTION(BlueprintCallable, Category = "Rade")
		void SetAnimArchetype(EAnimArchetype newAnimArchtype);


	// Recieve the anim from player Class
	void RecieveGlobalAnimID(EAnimState currentAnimCheck);

private:
	// Set new Animation
	void PlayLocalAnim(EAnimState currentAnimCheck);

public:


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//										State Checking

	/// Global Anim state Check
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
		bool IsAnimState(EAnimState currentAnimCheck);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
		bool IsAnimArchetype(EAnimArchetype AnimArchtypeCheck);

	// Check value from player class
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
		bool CanFireInAir();

	// Event called in blueprint when Jump_Start -> Jump_Idle , To enable fire in Air
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void InAirIdleStateEntered();


	// In air check from current anim state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Rade")
		bool IsInAir();


protected:


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//											 Custom Melee Sword

	// Start Trace of Sword Weapon
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void StartSwordWeaponTrace();

	// End Trace of Sword Weapon
	UFUNCTION(BlueprintCallable, Category = "Rade")
		void EndSwordWeaponTrace();




};
