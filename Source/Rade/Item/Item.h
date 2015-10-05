// Copyright 2015 Vagen Ayrapetyan

#pragma once

#include "GameFramework/Actor.h"
#include "RadeData.h"
#include "Item.generated.h"

UCLASS()
class RADE_API AItem : public AActor
{
	GENERATED_BODY()
public:	

	//			Basic Events
	AItem(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;



	//			Item Refs

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	//	TSubclassOf<AItem> SpawedItemArchtype;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Pickup")
		UStaticMesh* PickupMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Pickup")
		USkeletalMesh* PickupSkelMesh;


	UPROPERTY(Replicated)
	class ARadeCharacter* ThePlayer;


	//			Item Props

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		FString ItemName = FString("Name of Item");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		int32 ItemCount=1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		float Weight =1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
		TSubclassOf<AItemPickup> ItemPickupArchetype;


		float PickupScale=1;
	//			 Item Events


	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
		void ItemUsed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
		void ItemDroped(class AItemPickup* thePickup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
		void ItemUpdated();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		class ARadeCharacter* GetPlayer()const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "The HUD")
		class UWorld* GetPlayerWorld()const;


	virtual void InventoryUse(class ARadeCharacter* Player)
	{
		if (ThePlayer == Player || ThePlayer != Player)
		{
			ThePlayer = Player;
		}
		ItemUsed();

	};

	
};
