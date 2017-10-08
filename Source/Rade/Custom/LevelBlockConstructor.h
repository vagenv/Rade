// Copyright 2015-2017 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RadeData.h"
#include "LevelBlockConstructor.generated.h"


// Block COnstructor. Actor that holds, Constructs and Deconstructs Blocks
UCLASS()
class RADE_API ALevelBlockConstructor : public AActor
{
	GENERATED_BODY()
public:

	ALevelBlockConstructor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	// Save Block on End of Game ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor")
		bool bSaveBlocks = false;

	// Load Blocks on Start of Game?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructor")
		bool bLoadBlocks=false;

	// List of current Block in World
	UPROPERTY(ReplicatedUsing = ClientBlocksUpdated, EditAnywhere, BlueprintReadWrite, Category = "Constructor")
		TArray<FBlockData>  CurrentBlocks;

	// Blocks data updated, Called on client
	UFUNCTION()
		void ClientBlocksUpdated();

	// Blocks data update, called on server
	void Server_UpdateBlocksStatus();


	// The Game Mode Reference
	UPROPERTY()
		class ARadeGameMode* TheGM;


	// Add Block Into World
	virtual bool AddNewBlock(TSubclassOf <class ALevelBlock>  NewBlockArchtype, FVector& Loc, class AConstructorWeapon* TheConstructorWeapon);

	// Destroy Block
	virtual bool DestroyBlock(FVector Loc, AActor* TheInstigator);
	

};
