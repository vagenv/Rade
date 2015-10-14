// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class RADE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//							Basic Events

	ABaseCharacter(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;
	


	//							Basic Refs

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	class AInventory* TheInventory;


	UPROPERTY(ReplicatedUsing = CurrentWeaponUpdated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class AWeapon* TheWeapon;

	//							Basic Props


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
		float MaxHealth=100;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Rade Character")
		float Health=50;

	UPROPERTY(Replicated)
		bool bDead;


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
