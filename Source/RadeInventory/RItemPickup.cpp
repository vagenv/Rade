// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickup.h"
#include "RItem.h"
#include "RInventoryComponent.h"
#include "RadeUtil/RLog.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARItemPickup::ARItemPickup (const class FObjectInitializer& PCIP)
   : Super(PCIP)
{
   // Set Root Component
   RootComponent = PCIP.CreateDefaultSubobject<USceneComponent> (this, TEXT ("RootComponent"));
   RootComponent->SetIsReplicated (true);
   SetRootComponent (RootComponent);

/*
   // Set Skeletal Mesh Component
   SkeletalMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));
   SkeletalMesh->SetIsReplicated(true);
   SkeletalMesh->BodyInstance.SetCollisionProfileName("BlockAll");
   SkeletalMesh->SetSimulatePhysics(true);
   SkeletalMesh->SetupAttachment (GetRootComponent ());
   
   // Set Static Mesh Component
   Mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
   Mesh->SetIsReplicated(true);
   Mesh->BodyInstance.SetCollisionProfileName("BlockAll");
   Mesh->SetSimulatePhysics(true);
   Mesh->bAutoActivate = true;
   Mesh->SetupAttachment (GetRootComponent ());

   */

   // Set Trigger Component
   TriggerSphere = PCIP.CreateDefaultSubobject<USphereComponent> (this, TEXT("TriggerSphere"));
   TriggerSphere->InitSphereRadius (400);
   TriggerSphere->SetIsReplicated (true);
   TriggerSphere->BodyInstance.SetCollisionProfileName ("Pickup");
   TriggerSphere->SetupAttachment (GetRootComponent ());


   Inventory = PCIP.CreateDefaultSubobject<URInventoryComponent>(this, TEXT("Inventory"));
   Inventory->SetIsReplicated (true);

   bReplicates = true;
   PickupActivationDelay = 1.;
   bAutoPickup  = false;
   bAutoDestroy = true;
   SetReplicatingMovement (true);
}

// Replication
void ARItemPickup::GetLifetimeReplicatedProps (TArray<FLifetimeProperty> &OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps (OutLifetimeProps);

   DOREPLIFETIME (ARItemPickup, TriggerSphere);
   DOREPLIFETIME (ARItemPickup, MeshComponent);
   DOREPLIFETIME (ARItemPickup, Inventory);
   DOREPLIFETIME (ARItemPickup, PickupActivationDelay);
   DOREPLIFETIME (ARItemPickup, bAutoPickup);
   DOREPLIFETIME (ARItemPickup, bAutoDestroy);
}

void ARItemPickup::InitEmpty()
{
   UStaticMeshComponent *StaticMeshComponent
      = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass (), NAME_None, RF_Transient);
   FString defaultMeshPath = "/Game/Rade/Meshes/BasicMeshes/Shapes/Shape_Cube.Shape_Cube";
   UStaticMesh *StaticMesh = Cast<UStaticMesh>(StaticLoadObject (UStaticMesh::StaticClass (), NULL, *defaultMeshPath));
   StaticMeshComponent->SetStaticMesh (StaticMesh);

   MeshComponent = StaticMeshComponent;
   MeshComponent->SetupAttachment (GetRootComponent ());
   MeshComponent->RegisterComponent ();
   TriggerSphere->AttachToComponent (MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

   // Set Its Properties
   MeshComponent->SetMobility (EComponentMobility::Movable);
   MeshComponent->SetCollisionResponseToAllChannels (ECollisionResponse::ECR_Block);
   MeshComponent->SetCollisionEnabled (ECollisionEnabled::PhysicsOnly);
   MeshComponent->BodyInstance.SetCollisionProfileName ("BlockAll");
   MeshComponent->SetSimulatePhysics (true);
   MeshComponent->bAutoActivate = true;

   PickupActivationDelay = 1.;
   bAutoPickup  = false;
   bAutoDestroy = true;
}


void ARItemPickup::BeginPlay()
{
   Super::BeginPlay();

   // Enable overlap after a delay
   FTimerHandle MyHandle;
   GetWorldTimerManager().SetTimer (MyHandle,
                                    this,
                                    &ARItemPickup::ActivatePickupOverlap,
                                    PickupActivationDelay,
                                    false);
   /*
   // Disable Skeletal Mesh Outline
   if (SkeletalMesh) {
      SkeletalMesh->SetRenderCustomDepth(false);
      SkeletalMesh->SetIsReplicated(true);
   }

   // Disable Static Mesh Outline
   if (Mesh) Mesh->SetRenderCustomDepth(false);


   // If Mesh is Set , Activate Pickup
   if (  (Mesh         && Mesh->GetStaticMesh ())
      || (SkeletalMesh && SkeletalMesh->SkeletalMesh)) {
      ActivatePickupPhysics();

   // Else Activate it after a delay
   } else  {
      FTimerHandle MyActivatePhysicsHandle;
      GetWorldTimerManager().SetTimer(MyActivatePhysicsHandle, this, &AItemPickup::ActivatePickupPhysics, 0.1f, false);
   }
   */

   if (Inventory) {
       Inventory->OnInventoryUpdated.AddDynamic (this, &ARItemPickup::OnInventoryUpdate);
   }
}

/*

// Enable It as a Skeletal Mesh Pickup
bool ARItemPickup::SetAsSkeletalMeshPickup_Validate(){
   return true;
}
void ARItemPickup::SetAsSkeletalMeshPickup_Implementation()
{
   // TODO: Causes a crash. Still looking into it.
   // Set It as Root
   //SetRootComponent(SkeletalMesh);

   // Destroy Extra Component
   if (Mesh) Mesh->DestroyComponent();

   // Attach Trigger to Skeletal Mesh Component
   TriggerSphere->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

   // Set Its Properties
   SkeletalMesh->SetMobility(EComponentMobility::Movable);
   SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
   SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

   // Enable Replication of Its Movement
   SetReplicateMovement(true);
}

// Enable It as a Static Mesh Pickup
bool ARItemPickup::SetAsMeshPickup_Validate(){
   return true;
}
void ARItemPickup::SetAsMeshPickup_Implementation()
{
   // Set It as Root
   SetRootComponent(Mesh);

   // Destroy Extra Component
   if (SkeletalMesh) SkeletalMesh->DestroyComponent();

   // Attach Trigger to Static Mesh Component
   TriggerSphere->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);

   // Set Its Properties
   Mesh->SetMobility(EComponentMobility::Movable);
   Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
   Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

   // Enable Replication of Its Movement
   SetReplicateMovement(true);
}
*/


// Activate Overlap detection
void ARItemPickup::ActivatePickupOverlap ()
{

      /*
   // Check if Any Player is within the range to pickup this item
   if (GetLocalRole() >= ROLE_Authority && bAutoPickup) {

      for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
      {
         float distance = FVector::Distance (ActorItr->GetActorLocation (), GetActorLocation());
         if (distance > TriggerSphere->GetUnscaledSphereRadius()) continue;

         OnBeginOverlap (nullptr, *ActorItr, nullptr, 0, false, FHitResult());
      }
   }

   */

   // Enable Overlap Component
   TriggerSphere->OnComponentBeginOverlap.AddDynamic (this, &ARItemPickup::OnBeginOverlap);
   TriggerSphere->OnComponentEndOverlap.AddDynamic   (this, &ARItemPickup::OnEndOverlap);
}

// Activate Pickup Physics
void ARItemPickup::ActivatePickupPhysics ()
{
   /*
   // If Already activated (One of components Destroyed) Stop
   if (!Mesh || !SkeletalMesh) return;

   // Enable As Static Mesh Pickup
   if (Mesh && Mesh->GetStaticMesh()) SetAsMeshPickup();

   // Enable As Skeletal Mesh Pickup
   else if (SkeletalMesh && SkeletalMesh->SkeletalMesh) SetAsSkeletalMeshPickup();
   */
}

// Player Entered The Pickup Area
void ARItemPickup::OnBeginOverlap (UPrimitiveComponent* OverlappedComponent, 
                                   AActor* OtherActor, 
                                   UPrimitiveComponent* OtherComp, 
                                   int32 OtherBodyIndex, 
                                   bool bFromSweep, 
                                   const FHitResult & SweepResult)
{

   // Null or itself
   if (OtherActor == nullptr || OtherActor == this) return;
  
   // Does not have inventory
   URInventoryComponent *PlayerInventory = OtherActor->FindComponentByClass<URInventoryComponent>();
   if (PlayerInventory == nullptr) return;

   if (!bAutoPickup) {
      PlayerInventory->CurrentPickups.Add (this);
      PlayerInventory->OnPickupsUpdated.Broadcast ();

      // BP Event that player entered
      BP_PlayerEntered (OtherActor);
   } else {
      PlayerInventory->AddItem_Pickup (this);
   }

   /*
      // Auto give player the item
      if (bAutoPickup) {
         if (GetLocalRole() >= ROLE_Authority) PickedUp(radePlayer);

      // Wait player Input
      } else {
         // Set player ref of item pickup
         //Cast<ARadePlayer>(OtherActor)->currentPickup = this;
         if (radePlayer->IsLocallyControlled()) {
            if (SkeletalMesh != nullptr) SkeletalMesh->SetRenderCustomDepth(true);
            if (Mesh != nullptr)         Mesh->SetRenderCustomDepth(true);
         }
         if (GetLocalRole() >= ROLE_Authority)
            radePlayer->currentPickup = this;
      }
      
   }
   */
}

// Player Exited The Pickup Area
void ARItemPickup::OnEndOverlap (UPrimitiveComponent* OverlappedComponent,
                                 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex)
{
   // Null or itself
   if (OtherActor == nullptr || OtherActor == this) return;

   // Does not have inventory
   URInventoryComponent *PlayerInventory = OtherActor->FindComponentByClass<URInventoryComponent>();
   if (PlayerInventory == nullptr) return;

   PlayerInventory->CurrentPickups.RemoveSingle (this);
   PlayerInventory->OnPickupsUpdated.Broadcast ();

   // BP Event that player Exited
   BP_PlayerLeft (OtherActor);
      
   /*

   // Clean Pickup reference in player class
   if (GetLocalRole() >= ROLE_Authority && radePlayer->currentPickup == this)
   radePlayer->currentPickup = nullptr;

   // Enable Highlighting on Local Users
   if (radePlayer->IsLocallyControlled()) {
      if (SkeletalMesh != nullptr) SkeletalMesh->SetRenderCustomDepth(false);
      if (Mesh         != nullptr) Mesh->SetRenderCustomDepth(false);
   }
   */
}

// The Actual event of pickup
void ARItemPickup::PickedUp (AActor *InventoryOwner)
{
   if (!InventoryOwner) return;
   URInventoryComponent *PlayerInventory = InventoryOwner->FindComponentByClass<URInventoryComponent> ();
   if (!Inventory) return;

   if (bAutoDestroy) PlayerInventory->CurrentPickups.RemoveSingle (this);
   PlayerInventory->OnPickupsUpdated.Broadcast ();
   BP_PickedUp (InventoryOwner);
   if (bAutoDestroy) Destroy ();
}

void ARItemPickup::OnInventoryUpdate ()
{
   if (bAutoDestroy && Inventory && Inventory->GetItems ().Num () == 0) {
      Destroy ();
   }
}