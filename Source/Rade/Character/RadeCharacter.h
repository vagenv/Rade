// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Character.h"
#include "RadeCharacter.generated.h"

UCLASS()
class RADE_API ARadeCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//							Basic Events

	ARadeCharacter(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;
	virtual void PostBeginPlay();



	//							Basic Refs

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	class UInventory* TheInventory;


	UPROPERTY(ReplicatedUsing = CurrentWeaponUpdated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class AWeapon* TheWeapon;

	//							Basic Props


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
		float MaxHealth=100;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
		float Health=50;

	UPROPERTY(Replicated)
		bool bDead;


	// Character Name 
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Character")
		FString CharacterName;

	// Character Color
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Character")
		FLinearColor CharacterColor = FLinearColor::White;


	// Default inventory items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<TSubclassOf<class AItem> >  DefaultInventoryItems;


	//  Fall damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling")
		float FallAcceptableValue=1000;

	UPROPERTY(EditAnywhere, Category = "Falling")
		FRuntimeFloatCurve FallDamageCurve;

	//							Internal events


	// Called When Weapon Changed 
	UFUNCTION()
		virtual void CurrentWeaponUpdated(){};




	// Take Damage override
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)override;


	virtual void Landed(const FHitResult& Hit)override;




	// Death Events
	virtual void ServerDie();

	UFUNCTION(NetMulticast, Reliable)
		void GlobalDeath();
	virtual void GlobalDeath_Implementation();

	virtual void ForceRagdoll();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character State")
		void BP_Death();
	UFUNCTION(BlueprintImplementableEvent, Category = "Character State")
		void BP_Revive();



	//			Blueprint Events 
	
};
