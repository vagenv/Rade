// Copyright 2015 Vagen Ayrapetyan

#include "Rade.h"
#include "RadeGameMode.h"

#include "Custom/LevelBlockConstructor.h"
#include "Custom/SystemSaveGame.h"
#include "Custom/LevelBlock.h"
#include "Weapon/ConstructorWeapon.h"

#include "UnrealNetwork.h"


ALevelBlockConstructor::ALevelBlockConstructor(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		// A helper class object we use to find target UTexture2D object in resource package
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> NoteTextureObject;

		// Icon sprite category name
		FName ID_Notes;

		// Icon sprite display name
		FText NAME_Notes;

		FConstructorStatics()
			// Use helper class object to find the texture
			// "/Engine/EditorResources/S_Note" is resource path
			: NoteTextureObject(TEXT("/Engine/EditorResources/S_Note"))
			, ID_Notes(TEXT("Notes"))
			, NAME_Notes(NSLOCTEXT("SpriteCategory", "Notes", "Notes"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// We need a scene component to attach Icon sprite
	USceneComponent* SceneComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComp"));
	RootComponent = SceneComponent;
	RootComponent->Mobility = EComponentMobility::Static;

#if WITH_EDITORONLY_DATA
	SpriteComponent = PCIP.CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("Sprite"));
	if (SpriteComponent)
	{

		SpriteComponent->Sprite = ConstructorStatics.NoteTextureObject.Get();		// Get the sprite texture from helper class object
		SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_Notes;		// Assign sprite category name
		SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Notes;	// Assign sprite display name
		SpriteComponent->AttachParent = RootComponent;				        // Attach sprite to scene component
		SpriteComponent->Mobility = EComponentMobility::Static;
	}
#endif // WITH_EDITORONLY_DATA



	bReplicates = true;
}

void ALevelBlockConstructor::BeginPlay()
{
	Super::BeginPlay();
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Level Constructor started"));

	if (GetWorld() && GetWorld()->GetAuthGameMode() && GetWorld()->GetAuthGameMode<ARadeGameMode>())
	{
		GetWorld()->GetAuthGameMode<ARadeGameMode>()->TheLevelBlockConstructor = this;
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("New Constructor assigned"));
	}

	SetActorLocation(FVector(0, 0, 0));


	FTimerHandle MyHandle;
	GetWorldTimerManager().SetTimer(MyHandle, this, &ALevelBlockConstructor::PostBeginPlay, 1, false);


	
}

void ALevelBlockConstructor::PostBeginPlay()
{
	if (Role >= ROLE_Authority && bLoadBLocks)
	{
		//FTimerHandle MyHandle;
		//GetWorldTimerManager().SetTimer(MyHandle,this,&ALevelBlockConstructor::LoadBlocks,2,false);
		//LoadBlocks();
	}
}
void ALevelBlockConstructor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*

	UE_LOG(YourLog, Warning, TEXT("               "));
	UE_LOG(YourLog, Warning, TEXT("Level Block ConstructorEnded"));
	UE_LOG(YourLog, Warning, TEXT("               "));

	*/
	Super::EndPlay(EndPlayReason);
}

void ALevelBlockConstructor::ClientBlocksUpdated()
{
	/*
	for (int32 i = 0;i<CurrentBlocks.Num();i++)
	{
		if (CurrentBlocks.IsValidIndex(i))
		{
			printg("Update a box");
		}
	}
	*/

}

void ALevelBlockConstructor::Server_UpdateBlocksStatus()
{

	for (int32 i = 0;i<CurrentBlocks.Num();i++)
	{
		if (CurrentBlocks.IsValidIndex(i) )
		{
			// No Level item, respawn it
			if (GetWorld() && !CurrentBlocks[i].LevelItem && CurrentBlocks[i].Archetype && CurrentBlocks[i].Archetype.GetDefaultObject()
				&& Cast<ALevelBlock>(CurrentBlocks[i].Archetype.GetDefaultObject()))
			{
			//	printr();
				
				ALevelBlock* TheBlock=GetWorld()->SpawnActor<ALevelBlock>(CurrentBlocks[i].Archetype, CurrentBlocks[i].GlobalPosition, FRotator(0));
			
				if (TheBlock)
				{
					CurrentBlocks[i].LevelItem = TheBlock;
					TheBlock->AttachRootComponentToActor(this);
					//printr("Block Respawned ");
				}
				//else  printr("Bad Spawn");
				/*
				*/
				
				//AddNewBlock(Cast<ALevelBlock>(CurrentBlocks[i].Archetype.GetDefaultObject()), CurrentBlocks[i].GlobalPosition, NULL);
			}


			//printg("Update a box");
		}
	}
	
}

bool ALevelBlockConstructor::AddNewBlock(TSubclassOf <ALevelBlock>  NewBlockArchtype, FVector& Loc, AConstructorWeapon* TheConstructorWeapon)
{


	//printg("Add new block");
	UWorld* const World = GetWorld();
	if (NewBlockArchtype && World)
	{

		

		FVector tempV;
		tempV.X = round(Loc.X / 100);
		tempV.Y = round(Loc.Y / 100);
		tempV.Z = round(Loc.Z / 100);


		for (int32 i = 0; i < CurrentBlocks.Num(); i++)
		{
			if (CurrentBlocks[i].ConstructorPosition == tempV)
			{
				//printr("Position busy");
				return false;
			}
		}

		//Location Empty
		FBlockData temp = FBlockData();

		temp.Archetype = NewBlockArchtype;
		temp.ConstructorPosition = tempV;

		tempV.X *= 100;
		tempV.Y *= 100;
		tempV.Z *= 100;

		temp.GlobalPosition = tempV;
		temp.LevelItem = World->SpawnActor<ALevelBlock>(NewBlockArchtype, tempV, FRotator(0));

		if (!temp.LevelItem)
		{
			printr("Counldn't spawn level block");
			return false;
		}

		temp.LevelItem->ParentWeapon = TheConstructorWeapon;
		temp.LevelItem->AttachRootComponentToActor(this);
		temp.LevelItem->TheBlockConstructor = this;


		if (TheConstructorWeapon && TheConstructorWeapon->bAutoDestroBlocks)
		{
			temp.LevelItem->StartTimedRestore(TheConstructorWeapon, TheConstructorWeapon->BlockRestoreTime);
		}

		//printg("Block Spawned");
		CurrentBlocks.Add(temp);

		//Global_AddNewBlock(newBlock, Loc, TheConstructorWeapon);


		return true;

	}
	return false;
}

bool ALevelBlockConstructor::DestroyBlock(FVector Loc, AActor* TheWeapon)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		FVector tempV;
		tempV.X = round(Loc.X / 100);
		tempV.Y = round(Loc.Y / 100);
		tempV.Z = round(Loc.Z / 100);

		for (int32 i = 0; i < CurrentBlocks.Num(); i++)
		{
			if ( CurrentBlocks[i].ConstructorPosition == tempV)
			{
				if (CurrentBlocks[i].LevelItem)CurrentBlocks[i].LevelItem->Destroy();
				CurrentBlocks.RemoveAt(i);
				//Global_DestroyBlock(Loc, TheWeapon);
				//SaveBlocks();
				return true;
			}
		}
	}
	return false;
}
/*
void ALevelBlockConstructor::Global_DestroyBlock_Implementation(FVector Loc, AActor* TheWeapon)
{

	if (Role >= ROLE_Authority)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World)
	{
		FVector tempV;
		tempV.X = round(Loc.X / 100);
		tempV.Y = round(Loc.Y / 100);
		tempV.Z = round(Loc.Z / 100);

		for (int32 i = 0; i < CurrentBlocks.Num(); i++)
		{
			if (CurrentBlocks[i].ConstructorPosition == tempV)
			{
				if (CurrentBlocks[i].LevelItem)CurrentBlocks[i].LevelItem->Destroy();
				CurrentBlocks.RemoveAt(i);


				return;
		
			}
		}
	}

}



void ALevelBlockConstructor::Global_AddNewBlock_Implementation(ALevelBlock* newBlock, const FVector& Loc, class AConstructorWeapon* TheConstructorWeapon)
{


	
	if (Role >= ROLE_Authority) 
	{
		return;
	}



	//printg("Add new block");
	UWorld* const World = GetWorld();
	if (newBlock && World)
	{



		FVector tempV;
		tempV.X = round(Loc.X / 100);
		tempV.Y = round(Loc.Y / 100);
		tempV.Z = round(Loc.Z / 100);


		for (int32 i = 0; i < CurrentBlocks.Num(); i++)
		{
			if (CurrentBlocks.IsValidIndex(i) && CurrentBlocks[i].ConstructorPosition == tempV)
			{
				// Position is busy
				printr("Clinet : Position busy");
				//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Block Exist in current position"));
				return;
			}
		}

		//Location Empty
		FBlockData* temp = new FBlockData();

		temp.LevelItem = newBlock;

		if (TheConstructorWeapon && TheConstructorWeapon->BlockArchetype)
		{
			temp.Archetype = TheConstructorWeapon->BlockArchetype;
			BlockArchtype = TheConstructorWeapon->BlockArchetype;
			//temp.ArchetypePath = GetObjPath(theInstigator->BlockArchetype);
			//theInstigator->BlockArchetype->GetObjPath();

			//LoadLevelBlockFromPath(theInstigator->BlockArchetype);

			//print(temp.Archetype->GetName());
			//print("New block Archtype path"+temp.ArchetypePath.ToString());
		}
		//else printr("No Archtype in weapon");
		temp.ConstructorPosition = tempV;

		tempV.X *= 100;
		tempV.Y *= 100;
		tempV.Z *= 100;

		temp.GlobalPosition = tempV;
		temp.LevelItem = World->SpawnActor<ALevelBlock>(newBlock->GetClass(), tempV, FRotator(0));
		temp.LevelItem->ParentWeapon = Cast<AWeapon>(Instigator);
		temp.LevelItem->AttachRootComponentToActor(this);
		temp.LevelItem->parentConstructor = this;
		if (TheConstructorWeapon->bAutoDestroBlocks)
		{
			temp.LevelItem->StartTimedRestore(TheConstructorWeapon, TheConstructorWeapon->BlockRestoreTime);
		}



		CurrentBlocks.Add(*temp);
	}


}


*/


void ALevelBlockConstructor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALevelBlockConstructor, CurrentBlocks);
}