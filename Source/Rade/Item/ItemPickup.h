// Copyright 2015-2021 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "../RadeData.h"
#include "ItemPickup.generated.h"

// General pickup Class for Any Child of class "Item"
UCLASS()
class RADE_API AItemPickup : public AActor
{
	GENERATED_BODY()

public:

	AItemPickup(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;


	// Static Mesh
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		class UStaticMeshComponent* Mesh;

	// Skeletal Mesh
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		class USkeletalMeshComponent* SkeletalMesh;

	// Trigger Sphere 
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade")
		class USphereComponent* TriggerSphere;


	// Activation delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
		float PickupActivationDelay = 1;

	// Activate Pickup Overlap Tracking
	void ActivatePickupOverlap();

	// Activate Pickup Physics
	void ActivatePickupPhysics();


	// Activate As 
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Rade")
		void SetAsMeshPickup();
	bool SetAsMeshPickup_Validate();
	void SetAsMeshPickup_Implementation();

	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Rade")
		void SetAsSkeletalMeshPickup();
	bool SetAsSkeletalMeshPickup_Validate();
	void SetAsSkeletalMeshPickup_Implementation();


	// Auto Pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
		bool bAutoPickup;

	// Pickup Item Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
		TSubclassOf<class AItem> Item;

	// Override Default Item values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
		bool bOverideItemData = false;

	// The Override Data 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade")
		FItemData OverideItemData;

	// Player Entered
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Player Exited
	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent,class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Item was picked up by player
	void PickedUp(class ARadePlayer* Player);


	// BP Event that item Was Picked Up
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_PickedUp(class ARadePlayer* Player);

	// BP Event that Player Entered Pickup Area
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_PlayerEntered(class ARadePlayer* Player);

	// BP Event that Player Exited Pickup Area
	UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
		void BP_PlayerLeft(class ARadePlayer* Player);
};
