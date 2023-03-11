// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RCharacter.h"
#include "RSaveLib/RSaveInterface.h"
#include "InputActionValue.h"
#include "RPlayer.generated.h"

// For general input event
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInputEvent);

// Directional input event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRInputEventFloat, float, scrollValue);

class APlayerController;
class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class URTargetingComponent;

//=============================================================================
//                          Main Player Class
//=============================================================================
UCLASS(Blueprintable, BlueprintType, ClassGroup=(_Rade))
class RADE_API ARPlayer : public ARCharacter, public IRSaveInterface
{
   GENERATED_BODY()

public:

   //==========================================================================
   //                         Core
   //==========================================================================

   ARPlayer ();

   virtual void BeginPlay() override;
   virtual void EndPlay  (const EEndPlayReason::Type EndPlayReason) override;
   virtual void Tick     (float DeltaTime) override;
   void GetLifetimeReplicatedProps (TArray< FLifetimeProperty > & OutLifetimeProps) const override;

   //==========================================================================
   //                   Components and Important References
   //==========================================================================

   // Rade Player Controller
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Player")
      TObjectPtr<APlayerController> PlayerController;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      TObjectPtr<URTargetingComponent> TargetingComponent;

   //==========================================================================
   //            Camera
   //==========================================================================

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      TObjectPtr<USpringArmComponent> CameraBoomComponent;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      TObjectPtr<UCameraComponent> CameraComponent;

   // Camera Mouse sensitivity
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      float CameraMouseSensivity = 1.0;

   //==========================================================================
   //                  Input events
   //==========================================================================

protected:

   // Binding Player Input to internal events
   virtual void SetupPlayerInputComponent (class UInputComponent* InputComponent) override;

public:

   // --- Input event capture

   // MappingContext
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputAction> IA_TargetFocus;

   // --- Internal function callen on onput
	virtual void Input_Move (const FInputActionValue& Value);
	virtual void Input_Look (const FInputActionValue& Value);
   virtual void Input_Jump ();
   virtual void Input_TargetFocus ();

   //==========================================================================
   //                  Save/Load
   //==========================================================================

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;

public:
   //==========================================================================
   //                Util functions
   //==========================================================================

   // Get local Rade player for HUD
   UFUNCTION(BlueprintPure, Category = "Rade|Player", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Local Rade Player", CompactNodeTitle = "Get Rade Player", Keywords = "get rade player"))
      static ARPlayer* GetLocalRadePlayer (UObject* WorldContextObject);
};

