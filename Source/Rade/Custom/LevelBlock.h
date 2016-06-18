// Copyright 2015-2016 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "LevelBlock.generated.h"


// Base for Level Block
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
		class AConstructorWeapon* ParentWeapon;

	// Level Block Constructor
	UPROPERTY()
		class ALevelBlockConstructor* TheBlockConstructor;

	// If Block Must Restore After Some Item
		void StartTimedRestore(class AConstructorWeapon* newParentWeapon, float Time);

	// Restore The block back into weapon
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
