// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RadeData.h"
#include "ItemPickup.generated.h"


UCLASS()
class RADE_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	


	AItemPickup (const class FObjectInitializer& PCIP);


	virtual void BeginPlay() override;


	// Static Mesh
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	class UStaticMeshComponent* Mesh;

	// Skeletal Mesh
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	class USkeletalMeshComponent* SkeletalMesh;

	// Trigger Sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	class USphereComponent* TriggerSphere;
	

	// Activation delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		float PickupActivationDelay=1;

	// Activate Pickup Overlap Tracking
	void ActivatePickupOverlap();


	// Activate Pickup Physics
	void ActivatePickupPhysics();

	// Auto Pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		bool bAutoPickup;

	//Pickup Distance Checking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		float PickupDistance=600;

	// Pickup Item Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		TSubclassOf<class AItem> Item;

	// Override Default Item values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bOverideItemData = false;

	// The Override Data 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FItemData OverideItemData;


	// Player Entered
	UFUNCTION()
		void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Player Exited
	UFUNCTION()
		void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Item was picked up by player
	void PickedUp(AActor* Player);


	// BP Event that item Was Picked Up
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
		void BP_PickedUp(class ARadeCharacter* Player);

	// BP Event that Player Entered Pickup Area
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
		void BP_PlayerEntered(class ARadeCharacter* Player);

	// BP Event that Player Exited Pickup Area
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
		void BP_PlayerExited(class ARadeCharacter* Player);
};
