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

	UPROPERTY()
		class AWeapon* ParentWeapon;
	UPROPERTY()
		class UClass* ParentWeaponArchtype;
	UPROPERTY()
		class ALevelBlockConstructor* TheBlockConstructor;


	void StartTimedRestore(class AWeapon* newParentWeapon, float Time);
	void ReturnBlock();


	virtual void BeginPlay()override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemMesh")
		class USceneComponent* MyRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ItemMesh")
		class UStaticMeshComponent* Mesh;

	
	
};
