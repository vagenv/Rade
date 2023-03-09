// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RCharacter.h"
#include "RSaveLib/RSaveInterface.h"
#include "InputActionValue.h"
#include "RPlayer.generated.h"

// For general any events
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FREvent);

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
class URTargetableComponent;
class URTargetableMgr;

//=============================================================================
//                   Camera State Type
//=============================================================================

UENUM(BlueprintType)
enum class ECameraState : uint8
{
   FP_Camera UMETA(DisplayName = "First Person Camera"),
   TP_Camera UMETA(DisplayName = "Third Person Camera"),
};

//=============================================================================
//                          Main Player Class
//=============================================================================
UCLASS(config = Game)
class RADE_API ARPlayer : public ARCharacter, public IRSaveInterface
{
   GENERATED_BODY()

public:

   //==========================================================================
   //                         Core
   //==========================================================================

   ARPlayer();

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

   //==========================================================================
   //            1st person Mesh and animation
   //==========================================================================
   //  First Person Mesh
   UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Rade|Player")
      TObjectPtr<USkeletalMeshComponent> Mesh1P;

   //==========================================================================
   //            1st person and 3rd person cameras
   //==========================================================================

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      TObjectPtr<USpringArmComponent> ThirdPersonCameraBoom;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

   // Begin play Camera state
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      ECameraState DefaultCameraState = ECameraState::TP_Camera;

   // Current Camera State
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      ECameraState CurrentCameraState = ECameraState::TP_Camera;

   // Camera Mouse sensitivity
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Camera")
      float CameraMouseSensivity = 1.0;

   // Updates Component Visibility when Camera State Changes
   UFUNCTION()
      virtual void UpdateComponentsVisibility ();

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
	   TObjectPtr<UInputAction> IA_ChangeCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputAction> IA_Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
	   TObjectPtr<UInputAction> IA_AltAction;

   // --- Internal function callen on onput
	virtual void Input_Move (const FInputActionValue& Value);
	virtual void Input_Look (const FInputActionValue& Value);
   virtual void Input_Jump ();

   virtual void Input_ChangeCamera ();
   virtual void Input_Action ();
   virtual void Input_AltAction ();

   // For First person Camera
   virtual void FaceRotation (FRotator NewControlRotation, float DeltaTime) override;

   // --- Event to subscribe to
   UPROPERTY(BlueprintAssignable, Category = "Rade|Input")
      FRInputEvent Input_OnChangeCamera;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Input")
      FRInputEvent Input_OnAction;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Input")
      FRInputEvent Input_OnAltAction;

   //==========================================================================
   //                         Target
   //==========================================================================

   // Called by Tick. Adjusts camera angle
   UFUNCTION()
      virtual void TargetingTick (float DeltaTime);

   // Searches for new focus target
   UFUNCTION()
      virtual void TargetSearch ();

   // Handle to TargetCheck
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      FTimerHandle TargetCheckHandle;

   // Checks if Targeting actor is valid and within range
   UFUNCTION()
      virtual void TargetCheck ();


   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rade|Targetable")
      FRuntimeFloatCurve TargetAngleToLerpPower;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Targetable")
      URTargetableComponent* TargetCurrent = nullptr;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rade|Targetable")
      URTargetableMgr* TargetMgr = nullptr;

   UPROPERTY(BlueprintAssignable, Category = "Rade|Targetable")
      FREvent OnTargetUpdated;

   //==========================================================================
   //                         Save/Load
   //==========================================================================

protected:
   virtual void OnSave (FBufferArchive &SaveData) override;
   virtual void OnLoad (FMemoryReader &LoadData) override;

public:
   //==========================================================================
   //            Util functions
   //==========================================================================

   // Get local Rade player for HUD
   UFUNCTION(BlueprintPure, Category = "Rade|Player", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Local Rade Player", CompactNodeTitle = "Get Rade Player", Keywords = "get rade player"))
      static ARPlayer* GetLocalRadePlayer (UObject* WorldContextObject);
};

