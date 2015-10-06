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
	// Sets default values for this actor's properties
	AItemPickup (const class FObjectInitializer& PCIP);

	void PickedUp();
	void ActivatePickup();

	virtual void BeginPlay() override;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		float PickupCollisionDelay=1;
	void ActivatePickupOverlap();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		bool bAutoPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		float PickupDistance=600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
		TSubclassOf<class AItem> Item;

	///

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bOverideItemData = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FItemData OverideItemData;


	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
		class UStaticMeshComponent* Mesh;
	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
		class USphereComponent* TriggerSphere;

	UFUNCTION()
		void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PickedUp(AActor* Player);
	
};
