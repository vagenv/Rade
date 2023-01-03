// Copyright 2015-2023 Vagen Ayrapetyan

#include "RItemPickup.h"
#include "RItem.h"
#include "RInventoryComponent.h"
#include "RUtilLib/RLog.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARItemPickup::ARItemPickup ()
{
   // Set Root Component
   RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
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
   TriggerSphere = CreateDefaultSubobject<USphereComponent> (TEXT("TriggerSphere"));
   TriggerSphere->InitSphereRadius (400);
   TriggerSphere->SetIsReplicated (true);
   TriggerSphere->BodyInstance.SetCollisionProfileName ("Pickup");
   TriggerSphere->SetupAttachment (GetRootComponent ());


   Inventory = CreateDefaultSubobject<URInventoryComponent>(TEXT("Inventory"));
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

void ARItemPickup::InitEmpty ()
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
}

void ARItemPickup::BeginPlay ()
{
   Super::BeginPlay ();

   // Enable overlap after a delay
   FTimerHandle MyHandle;
   GetWorldTimerManager().SetTimer (MyHandle,
                                    this,
                                    &ARItemPickup::ActivatePickupOverlap,
                                    PickupActivationDelay,
                                    false);
   /*

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

// Enable It as a Static Mesh Pickup
bool ARItemPickup::SetAsMeshPickup_Validate(){
   return true;
}
void ARItemPickup::SetAsMeshPickup_Implementation()
{
   // Set It as Root
   SetRootComponent(Mesh);

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

   // Only Inventory containing actors
   URInventoryComponent *PlayerInventory = OtherActor->FindComponentByClass<URInventoryComponent>();
   if (PlayerInventory == nullptr) return;

   if (bAutoPickup) {
      URInventoryComponent::TransferAll (Inventory, PlayerInventory);
   } else {
      PlayerInventory->Pickup_Add (this);

      // BP Event that player entered
      BP_PlayerEntered (OtherActor);
   }
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

   PlayerInventory->Pickup_Rm (this);

   // BP Event that player Exited
   BP_PlayerLeft (OtherActor);
}

// The Actual event of pickup
void ARItemPickup::PickedUp (AActor *InventoryOwner)
{
   // if (!InventoryOwner) return;
   // URInventoryComponent *PlayerInventory = InventoryOwner->FindComponentByClass<URInventoryComponent> ();
   // if (!Inventory) return;

   // if (bAutoDestroy) PlayerInventory->CurrentPickups.RemoveSingle (this);
   // PlayerInventory->OnPickupsUpdated.Broadcast ();
   // BP_PickedUp (InventoryOwner);
   // if (bAutoDestroy) Destroy ();
}

void ARItemPickup::OnInventoryUpdate ()
{
   if (bAutoDestroy && Inventory->GetItems ().Num () == 0) {
      Destroy ();
   }
}

