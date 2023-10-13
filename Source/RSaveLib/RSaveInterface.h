// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"

#include "RSaveInterface.generated.h"

class UWorld;
class URSaveGame;
class URWorldSaveMgr;

// This must be added to header of class
/*

   //==========================================================================
   //                 Save / Load
   //==========================================================================
public:
   // Status Saved / Loaded between sessions.
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Save")
      bool bSaveLoad = false;

	// Should be called during BeginPlay
	UFUNCTION()
      void ConnectToSaveMgr ();

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader  &LoadData) override;
*/

// This must be added to source of class
/*
void T::BeginPlay ()
{
	Super::BeginPlay ();

	ConnectToSaveMgr ();
}

void T::ConnectToSaveMgr ()
{
	if (!bSaveLoad || !R_IS_NET_ADMIN) return;

   // Careful with collision of 'UniqueSaveId'
   FString UniqueSaveId = GetOwner ()->GetName () + "_??????????????";

	if (!InitSaveInterface (this, UniqueSaveId)) {
		FTimerHandle RetryHandle;
		RTIMER_START (RetryHandle, this, &T::ConnectToSaveMgr, 1, false);
	}
}

void T::OnSave (FBufferArchive &SaveData)
{
}

void T::OnLoad (FMemoryReader &LoadData)
{
}
*/




// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class URSaveInterface : public UInterface
{
	GENERATED_BODY()
};

class RSAVELIB_API IRSaveInterface
{
	GENERATED_BODY()

protected:

	virtual bool InitSaveInterface (const UObject* WorldContextObject, const FString &SaveId_);

	// --- Must be implemented
	virtual void OnSave (FBufferArchive &SaveData) = 0;
	virtual void OnLoad (FMemoryReader  &LoadData) = 0;

	// Internal events
   UFUNCTION()
      virtual void OnSave_Internal (URSaveGame* SaveGame);

   UFUNCTION()
      virtual void OnLoad_Internal (URSaveGame* SaveGame);

private:

	// Values for internal tracking
	FString SaveId;

	//UPROPERTY()
		TWeakObjectPtr<URWorldSaveMgr> WorldSaveMgr = nullptr;
};
