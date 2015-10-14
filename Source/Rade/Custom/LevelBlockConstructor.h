// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RadeData.h"
#include "LevelBlockConstructor.generated.h"
//#include "Custom/LevelBlock.h"



UCLASS()
class RADE_API ALevelBlockConstructor : public AActor
{
	GENERATED_BODY()
public:
	ALevelBlockConstructor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;
	virtual void PostBeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
		// A UBillboardComponent to hold Icon sprite
		UBillboardComponent* SpriteComponent;

	// Icon sprite
	UPROPERTY()
		UTexture2D* SpriteTexture;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor")
		bool bSaveBlocks = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor")
		bool bLoadBlocks=false;

	UPROPERTY(ReplicatedUsing = ClientBlocksUpdated, EditAnywhere, BlueprintReadWrite, Category = "Constructor")
		TArray<FBlockData>  CurrentBlocks;

	UFUNCTION()
		void ClientBlocksUpdated();

	void Server_UpdateBlocksStatus();

	//TSubclassOf <class ALevelBlock> BlockArchtype;

	UPROPERTY()
		class ARadeCharacter* ThePlayer;
	UPROPERTY()
		class ARadeGameMode* TheGM;



	virtual bool AddNewBlock(TSubclassOf <class ALevelBlock>  NewBlockArchtype, FVector& Loc, class AConstructorWeapon* TheConstructorWeapon);
	virtual bool DestroyBlock(FVector Loc, AActor* TheWeapon);
	



private:

	/*
	//NetMulticast
	UFUNCTION(NetMulticast, Reliable)
		void Global_AddNewBlock(ALevelBlock* newBlock, const FVector& Loc, class AConstructorWeapon* TheConstructorWeapon);
	virtual void Global_AddNewBlock_Implementation(ALevelBlock* newBlock, const FVector& Loc, class AConstructorWeapon* TheConstructorWeapon);
	
	UFUNCTION(NetMulticast, Reliable)
		void Global_DestroyBlock(FVector Loc, AActor* TheWeapon);
	virtual void Global_DestroyBlock_Implementation(FVector Loc, AActor* TheWeapon);
	*/
};
