// Copyright 2015-2021 Vagen Ayrapetyan

#include "Weapon.h"
#include "../Item/Inventory.h"
#include "../Character/RadePlayer.h"
#include "../Rade.h"
#include "../Character/RadeAnimInstance.h"

#include "Net/UnrealNetwork.h"

#include "../RadePC.h"


AWeapon::AWeapon(const class FObjectInitializer& PCIP)
   : Super(PCIP)
{
   RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root Component"));

   Mesh1P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh1P"));
   Mesh1P->CastShadow = false;
   Mesh1P->SetupAttachment(RootComponent, NAME_None);
   Mesh1P->bOnlyOwnerSee = true;
   Mesh1P->SetVisibility(true);
   Mesh1P->SetIsReplicated(true);

   Mesh3P = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh3P"));
   Mesh3P->CastShadow = true;
   Mesh3P->SetupAttachment(RootComponent, NAME_None);
   Mesh3P->bOwnerNoSee = true;
   Mesh3P->SetVisibility(true);
   Mesh3P->SetIsReplicated(true);

   bReplicates = true;
   ItemName = "Weapon";
}

void AWeapon::BeginPlay()
{
   Super::BeginPlay();

   // Double check at the beginning
   bReloading = false;
   bShooting  = false;
}

// Get Fire Socket 
FTransform AWeapon::GetFireSocketTransform()
{
   if        (ThePlayer->CurrentCameraState == ECameraState::FP_Camera) return Mesh1P->GetSocketTransform(FireSocketName);
   else if (ThePlayer->CurrentCameraState == ECameraState::TP_Camera) return Mesh3P->GetSocketTransform(FireSocketName);
   else return FTransform();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Main Fire

// Player Pressed Fire
void AWeapon::FireStart()
{
   // BP Fire Started
   BP_FirePress_Start();

   // Shooting Enabled
   bShooting = true;

   // If not currently in main and alternative shooting mode/delay  call The Fire event
   if (  ThePlayer
      && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreFireTimeHandle)
      && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
      ThePlayer->GetWorldTimerManager().SetTimer(PreFireTimeHandle, this, &AWeapon::PreFire, MainFire.FireSpeed, true, 0);
}

// Player Released Fire
void AWeapon::FireEnd()
{
   // BP Fire Released
   BP_FirePress_End();

   // Shooting Disabled
   bShooting = false;
}

// Pre Fire
void AWeapon::PreFire()
{
   // BP Pre Fire
   BP_PreFire();

   if (!ThePlayer)return;
   
   // If player stopped shooting stop event
   if (!bShooting) {
      ThePlayer->GetWorldTimerManager().ClearTimer(PreFireTimeHandle);
      return;
   }

   // Currently Equipping this weapon, delay a bit
   if (ThePlayer->IsAnimState(EAnimState::Equip)) {
      // try after a small delay
      FTimerHandle MyHandle;
      ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::PreFire, 0.2, false);
      return;
   }

   // Wrong Weapon or Player Anim State
   if (!CanShoot())return;

   // Ammo Check
   if (!MainFire.CanFire()) {
      //  BP No Ammo
      BP_NoAmmo();

      // if have ammo, start reloading
      if (CanReload())ReloadWeaponStart();
      return;
   }


   // The real fire event
   Fire();

   // Use Ammo
   if (bUseAmmo)UseMainFireAmmo();


   // Set Player Anim State
   ThePlayer->ServerSetAnimID(EAnimState::Fire);

   // Decrease move speed when shooting
   ThePlayer->ResetMoveSpeed();

   ///   Stop Fire Anim
   FTimerHandle MyHandle;
   ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::StopFireAnim, 0.1, false);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Alternative Fire

// Alt Fire Pressed
void AWeapon::AltFireStart()
{
   // IF alt fire enabled
   if (!bAltFireEnabled)return;

   // If not currently in main and alternative shooting mode/delay , call the Pre Fire event
   BP_AltFirePress_Start();
   if (  !ThePlayer->GetWorldTimerManager().IsTimerActive(PreFireTimeHandle)
      && !ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
      ThePlayer->GetWorldTimerManager().SetTimer(PreAltFireTimeHandle, this, &AWeapon::PreAltFire, AlternativeFire.FireSpeed, true, 0);

   // Start shooting
   bShooting = true;
}

// Alt Fire Released
void AWeapon::AltFireEnd()
{
   // Stop Shooting
   bShooting = false;

   // BP Stop Alt Fire
   BP_AltFirePress_End();

   // Clear Fire Timer
   if (ThePlayer->GetWorldTimerManager().IsTimerActive(PreAltFireTimeHandle))
      ThePlayer->GetWorldTimerManager().ClearTimer(PreAltFireTimeHandle);

}

// Pre Alt Fire
void AWeapon::PreAltFire()
{
   if (!ThePlayer)return;

   // Bp Pre Alternative Fire
   BP_PreAltFire();

   // If not shooting , stop everything
   if (!bShooting) {
      ThePlayer->GetWorldTimerManager().ClearTimer(PreAltFireTimeHandle);
      return;
   }

   // Currently Equipping this weapon
   if (  ThePlayer->ArmsAnimInstance
      && ThePlayer->ArmsAnimInstance->IsAnimState(EAnimState::Equip))
   {
      // Try after a small delay
      FTimerHandle MyHandle;
      ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::PreAltFire, 0.2, false);
      return;
   }

   // Wrong Player Anim State
   if (!CanShoot())return;

   // Separate Ammo check
   if (bAltFireSeparateAmmo) {
      if (!AlternativeFire.CanFire()) {
         // BP no ammo for Alt fire
         BP_NoAmmo();
         return;
      }

   // Unified ammo Check
   } else if ( AlternativeFire.FireCost > 0
            && AlternativeFire.FireCost > MainFire.CurrentAmmo)
   {
         BP_NoAmmo();
         return;
   }

   // The real Alt fire event
   AltFire();

   // Use Alt Fire Ammo
   if (bUseAmmo)UseAltFireAmmo();

   // Set Player Anim State
   ThePlayer->ServerSetAnimID(EAnimState::Fire);

   // Decrease move speed when shooting
   ThePlayer->ResetMoveSpeed();

   ///   Stop Fire Anim
   FTimerHandle MyHandle;
   ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::StopFireAnim, 0.1, false);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Equip/UnEquip Part

// Equip Start
void AWeapon::EquipStart()
{
   if (!ThePlayer)return;

   // BP Equip Start
   BP_Equip_Start();

   // Attach First person Mesh
   if (Mesh1P)
      AttachToComponent(ThePlayer->Mesh1P, FAttachmentTransformRules::KeepRelativeTransform, FName("WeaponSocket"));

   // Attach Third Person Mesh
   if (Mesh3P)   
      Mesh3P->AttachToComponent(ThePlayer->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("WeaponSocket"));
   
   // Load Weapon Data from inventory
   if (ThePlayer && ThePlayer->TheInventory) {
      for (int32 i = 0; i < ThePlayer->TheInventory->Items.Num(); i++) {

         if (  ThePlayer->TheInventory->Items.IsValidIndex(i)
            && ThePlayer->TheInventory->Items[i].Archetype
            && ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()
            && ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()->GetClass() == GetClass())
         {
            MainFire        = ThePlayer->TheInventory->Items[i].MainFireStats;
            AlternativeFire = ThePlayer->TheInventory->Items[i].AltFireStats;
            break;         
         }
      }
   }

   // Delay equip end
   FTimerHandle myHandle;
   ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::EquipEnd, EquipTime, false);
}

// Equip Ended
void AWeapon::EquipEnd()
{
   // BP equip Ended
   BP_Equip_End();

   // Save Inventory
   if (ThePlayer && ThePlayer->TheInventory) 
      ThePlayer->TheInventory->SaveInventory();
}

// Unequip Start
void AWeapon::UnEquipStart()
{
   // Save the weapon stats
   SaveCurrentWeaponStats();

   // BP UnEquip Start
   BP_Unequip_Start();

   //delay unequip End
   FTimerHandle myHandle;
   ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::UnEquipEnd, EquipTime, false);
}

// Unequip End
void AWeapon::UnEquipEnd()
{
   // BP unequip End
   BP_Unequip_End();

   // Save Inventory
   if (ThePlayer && ThePlayer->TheInventory) ThePlayer->TheInventory->SaveInventory();

   // Destroy the Weapon Actor
   Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Reload Part

// Reload Start
void AWeapon::ReloadWeaponStart()
{
   if (!ThePlayer)return;

   // BP reload Start
   BP_Reload_Start();

   // Set reload State in weapon adn player
   bReloading = true;
   ThePlayer->ServerSetAnimID(EAnimState::Reload);

   // Delay reload End
   FTimerHandle myHandle;
   ThePlayer->GetWorldTimerManager().SetTimer(myHandle, this, &AWeapon::ReloadWeaponEnd, ReloadTime, false);
}

// reload Ended
void AWeapon::ReloadWeaponEnd()
{
   // BP Reload Ended
   BP_Reload_End();

   // Subtract clip number
   MainFire.ClipNumber--;

   // Set Current Ammo to clip size
   MainFire.CurrentAmmo = MainFire.ClipSize;

   // Stop Fire State in weapon and Player
   bReloading = false;
   ThePlayer->ServerSetAnimID(EAnimState::Idle_Run);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 State Checking

// Can Weapon Reload ?
bool AWeapon::CanReload()
{
   if (bReloading || !ThePlayer || !MainFire.CanReload()) return false;
   return true;
}

// Can Weapon and Player Fire
bool AWeapon::CanShoot()
{
   if (!ThePlayer || !ThePlayer->CanShoot()) return false;
   return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Additional Events

// Save Weapon Data
void AWeapon::SaveCurrentWeaponStats()
{
   if (!ThePlayer || !ThePlayer->TheInventory)return;

   // Find Weapon data in item list
   for (int32 i = 0; i < ThePlayer->TheInventory->Items.Num(); i++) {
      
      if (  ThePlayer->TheInventory->Items.IsValidIndex(i)
         && ThePlayer->TheInventory->Items[i].Archetype->GetDefaultObject()->GetClass() == GetClass())
      {
         // Set The Inventory item data to this weapon data
         ThePlayer->TheInventory->Items[i].MainFireStats = MainFire;
         ThePlayer->TheInventory->Items[i].AltFireStats = AlternativeFire;
         return;
      }
   }
}

// Stop Fire Anim after some time
void AWeapon::StopFireAnim()
{
   if (!ThePlayer) return;

   //IsMovingOnGround
   if (  ThePlayer->CharacterMovementComponent
      && ThePlayer->CharacterMovementComponent->IsMovingOnGround()) {
      ThePlayer->ServerSetAnimID(EAnimState::Idle_Run);
   } else {
      //print("End shoot in air");
      ThePlayer->ServerSetAnimID(EAnimState::Jumploop);
   }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Ammo Part

// Use Main Fire Ammo
void AWeapon::UseMainFireAmmo()
{
   // BP Ammo Used
   BP_Ammo_Used();

   // Subtract Fire Cost
   MainFire.CurrentAmmo -= MainFire.FireCost;
}

// use Alt Fire Ammo
void  AWeapon::UseAltFireAmmo()
{
   // BP Alt Ammo Used
   BP_AltAmmo_Used();

   // Check which ammo to subtract from
   if (bAltFireSeparateAmmo) AlternativeFire.CurrentAmmo -= AlternativeFire.FireCost;
   else                       MainFire.CurrentAmmo -= AlternativeFire.FireCost;
}

// Add Ammo from weapon pointer
void AWeapon::AddAmmo(AWeapon* weapAmmo)
{
   if (weapAmmo) {
      MainFire.AddAmmo (weapAmmo->MainFire.CurrentAmmo, weapAmmo->MainFire.ClipNumber);
   }
}

// Add Ammo as a number
void AWeapon::AddAmmo(float newAmmo, int32 newClip)
{
   MainFire.AddAmmo(newAmmo,newClip);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                 Melee Attack 

// Melee Attack Start
void AWeapon::PreMeleeAttack()
{
   BP_PreMeleeAttack();
   if (bShooting || bMeleeAttacking)return;

   bMeleeAttacking = true;

   // Reset After Delay
   FTimerHandle MyHandle;
   ThePlayer->GetWorldTimerManager().SetTimer(MyHandle, this, &AWeapon::ResetMeleeAttack, MeleeAttackSpeed, false);

   // Set Anim State
   if (ThePlayer) ThePlayer->ServerSetAnimID(EAnimState::Melee_Attack);

   // Call Actual Event
   MeleeAttack();
}

// Actual Melee Attack
void AWeapon::MeleeAttack()
{
   BP_MeleeAttack();

   if (Mesh1P && ThePlayer && ThePlayer->Controller) {
      FVector  CamLoc;
      FRotator CamRot;
      ThePlayer->Controller->GetPlayerViewPoint(CamLoc, CamRot);

      /// Get Camera Location and Rotation
      FVector Camoffset  = ThePlayer->FirstPersonCameraComponent->GetRelativeLocation();
      FVector TraceStart = Camoffset + ThePlayer->GetActorLocation() - CamRot.Vector() * 20;
      FVector TraceEnd   = Camoffset + ThePlayer->GetActorLocation() + CamRot.Vector()  * MeleeAttackDistance;

      // Set trace values and perform trace to retrieve hit info
      FCollisionQueryParams TraceParams(FName(TEXT("Melee Weapon Trace")), true, this);
      TraceParams.bReturnPhysicalMaterial = true;
      TraceParams.AddIgnoredActor(ThePlayer);
      TraceParams.AddIgnoredActor(this);

      FHitResult FHit(ForceInit);
      GetWorld()->LineTraceSingleByChannel(FHit, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams);

      ARadeCharacter* EnemyPlayer = Cast<ARadeCharacter>(FHit.GetActor());

      // Forward Hit Checking
      if (EnemyPlayer) {
         if (EnemyPlayer) {
            // Hit Enemy
            BP_HitEnemy(FHit);

            // Apply Damage
            UGameplayStatics::ApplyDamage(EnemyPlayer, MeleeAttackDamage, ThePlayer->Controller, Cast<AActor>(this), UDamageType::StaticClass());
         }
      } else {
         // Right Hit Checking
         FHitResult RHit(ForceInit);

         FVector rightAngle = CamRot.Vector().RotateAngleAxis(MeleeAttackAngles, FVector::UpVector);
                 rightAngle = Camoffset + ThePlayer->GetActorLocation() + rightAngle * MeleeAttackDistance;

         GetWorld()->LineTraceSingleByChannel(RHit, TraceStart, rightAngle, ECC_WorldStatic, TraceParams);

         EnemyPlayer = Cast<ARadeCharacter>(RHit.GetActor());
         if (EnemyPlayer) {

            BP_HitEnemy(RHit);
            // Apply Damage
            UGameplayStatics::ApplyDamage(EnemyPlayer, MeleeAttackDamage, ThePlayer->Controller, Cast<AActor>(this), UDamageType::StaticClass());
         } else  {
            // Left Hit Checking
            FHitResult LHit(ForceInit);

            FVector leftAngle = CamRot.Vector().RotateAngleAxis(-MeleeAttackAngles, FVector::UpVector);
                    leftAngle = Camoffset + ThePlayer->GetActorLocation() + leftAngle * MeleeAttackDistance;

            GetWorld()->LineTraceSingleByChannel(LHit, TraceStart, leftAngle, ECC_WorldStatic, TraceParams);

            EnemyPlayer = Cast<ARadeCharacter>(LHit.GetActor());
            if (EnemyPlayer) {
               BP_HitEnemy(LHit);

               // Apply Damage
               UGameplayStatics::ApplyDamage(EnemyPlayer, MeleeAttackDamage, ThePlayer->Controller, Cast<AActor>(this), UDamageType::StaticClass());
            }
         }
      }
   }
}

void AWeapon::ResetMeleeAttack()
{
   bMeleeAttacking = false;
   if (ThePlayer)ThePlayer->ServerSetAnimID(EAnimState::Idle_Run);
}


void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME(AWeapon, MainFire);
   DOREPLIFETIME(AWeapon, AlternativeFire);
   DOREPLIFETIME(AWeapon, Mesh1P);
   DOREPLIFETIME(AWeapon, Mesh3P);
}