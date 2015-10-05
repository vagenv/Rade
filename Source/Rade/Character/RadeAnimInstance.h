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

	bool bOriginalAnimInstance=false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		EAnimArchetype AnimArchetype;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Anim State")
		EAnimState PlayerCurrentAnimState;

	UPROPERTY()
		class ARadeCharacter* ThePlayer;


	UFUNCTION(BlueprintCallable, Category = "Player Anim State")
		void AnimEnded(EAnimState currentAnimCheck);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Anim State")
		void AnimStartedBPEvent(EAnimState currentAnimCheck);


	void PlayLocalAnim(EAnimState currentAnimCheck);

	void RecieveGlobalAnimID(EAnimState currentAnimCheck);


	/// Global Anim Check
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsAnimState(EAnimState currentAnimCheck);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsAnimArchetype(EAnimArchetype AnimArchtypeCheck);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool CanFireInAir();

	UFUNCTION(BlueprintCallable, Category = "Player Anim State")
		void InAirIdleStateEntered();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		EAnimState GetCurrentAnimID();



	bool bInAir;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Anim State")
		bool IsInAir();

	//UFUNCTION(BlueprintImplementableEvent, Category = "Anim Events")
	//	void SetFireAnimStart(bool value);


	
};
