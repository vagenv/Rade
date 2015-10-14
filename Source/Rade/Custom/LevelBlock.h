// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "LevelBlock.generated.h"

UCLASS()
class RADE_API ALevelBlock : public AActor
{
	GENERATED_BODY()
	
public:

	ALevelBlock(const class FObjectInitializer& PCIP);
	virtual void BeginPlay()override;


	// Restore Amoo after block Destroy
	bool bRestoreWeaponAmmo = false;

	// The weapon That Spawned The Bllock
	UPROPERTY()
		class AWeapon* ParentWeapon;

	// Level Block Constructor
	UPROPERTY()
		class ALevelBlockConstructor* TheBlockConstructor;

	// If Block Must Restore After Some Item
	void StartTimedRestore(class AWeapon* newParentWeapon, float Time);
	void ReturnBlock();
	
	// Event in Blueprint that Block retured to weapon
	UFUNCTION(BlueprintImplementableEvent, Category = "Block")
		void BP_BlockReturned();


	// Block Root
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
		class USceneComponent* MyRoot;

	// Block Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
		class UStaticMeshComponent* Mesh;

	
	
};
