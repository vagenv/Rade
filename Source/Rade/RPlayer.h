// Copyright 2015-2023 Vagen Ayrapetyan

#pragma once

#include "RCharacter.h"
#include "InputActionValue.h"
#include "RPlayer.generated.h"

// For general input event
DECLARE_DYNAMIC_MULTICAST_DELEGATE (FRInputEvent);

// Directional input event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FRInputEventFloat, float, scrollValue);


class APlayerController;
class URJetpackComponent;
// class URAnimInstance;
class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;

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
class RADE_API ARPlayer : public ARCharacter
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


   //==========================================================================
   //                   Components and Important References
   //==========================================================================

   // Rade Player Controller
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Player")
      TObjectPtr<APlayerController> PlayerController;

   // Rade Player Controller
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Player")
      TObjectPtr<URJetpackComponent> Jetpack;

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

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
      TObjectPtr<UInputAction> IA_Save;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rade|Input", meta = (AllowPrivateAccess = "true"))
      TObjectPtr<UInputAction> IA_Load;

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
   //                         Save/Load
   //==========================================================================

   // Input events
   void Input_SaveGame ();
   void Input_LoadGame ();

   // Callbacks, called by USaveMgr
   UFUNCTION ()
      void OnSave ();
   UFUNCTION ()
      void OnLoad ();

public:

   //==========================================================================
   //         Network Chat And Properties
   //==========================================================================

   /*

   // Character Name
   UPROPERTY(ReplicatedUsing = OnRep_CharacterStatsUpdated, EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
      FString CharacterName;

   // Character Color
   UPROPERTY(ReplicatedUsing = OnRep_CharacterStatsUpdated , EditAnywhere, BlueprintReadWrite, Category = "Rade|Character")
      FLinearColor CharacterColor = FLinearColor::White;

   // Character Stats Updated
   UFUNCTION()
      virtual void OnRep_CharacterStatsUpdated();
   // BP Server Event - Character Died
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade|Character")
      void BP_CharacterStatsUpdated();

   // Set Character Stats
   UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Rade|Character")
      void SetCharacterStats(const FString & newName, FLinearColor newColor);
   bool SetCharacterStats_Validate(const FString & newName, FLinearColor newColor);
   void SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor);

   // Add Chat Message
   UFUNCTION(Reliable, Server, WithValidation, BlueprintCallable, Category = "Rade")
      void AddChatMessage(const FString & TheMessage);
   bool AddChatMessage_Validate(const FString & TheMessage);
   void AddChatMessage_Implementation(const FString & TheMessage)   ;

   virtual void SetCharacterStats_Implementation(const FString & newName, FLinearColor newColor);
   */


   //==========================================================================
   //         Damage, Death and Revive
   //==========================================================================

   //virtual void Die (class AActor *DeathCauser, class AController* EventInstigator) override;

   //==========================================================================
   //         Death and Revive
   //==========================================================================
   /*

   // Called in BP when player died
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_PlayerDied();

   // Called in BP when player revived
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void BP_PlayerRevived();

protected:

   // Called When Player Revives
   virtual void ServerRevive()override;

   // Called On Server when player Died
   virtual void ServerDie()override;

   // Called on all users when player died
   virtual void GlobalDeath_Implementation()override;

   // Called on all users when player revived
   virtual void GlobalRevive_Implementation();
   */
public:

   void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

   //==========================================================================
   //                       State Checking
   //==========================================================================

   /*
   // Called to reset current move speed when player fired (Fire after sprint)
   UFUNCTION(BlueprintImplementableEvent, Category = "Rade")
      void ResetMoveSpeed();

   // Check player state if he can fire
   virtual bool CanShoot();

   // Can player Sprint
   UFUNCTION(BlueprintCallable, Category = "Rade")
      bool  CanSprint();

   // Player Landed
   virtual void Landed(const FHitResult& Hit)override;
   */

public:
   //==========================================================================
   //            Util functions
   //==========================================================================

   // Get local Rade player for HUD
   UFUNCTION(BlueprintPure, Category = "Rade|Player", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DisplayName = "Get Local Rade Player", CompactNodeTitle = "Get Rade Player", Keywords = "get rade player"))
      static ARPlayer* GetLocalRadePlayer (UObject* WorldContextObject);
};

