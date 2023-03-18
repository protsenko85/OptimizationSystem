
#include "PhysInstancedStaticMeshComponent.h"

#include <PxPhysics.h>

#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
//#include "Private/PhysicsEngine/PhysXSupport.h"

UPhysInstancedStaticMeshComponent::UPhysInstancedStaticMeshComponent(const FObjectInitializer& initializer)
    : Super(initializer)
{
    this->OnComponentHit.AddDynamic(this, &UPhysInstancedStaticMeshComponent::OnHit);
    
    // Tick needed for updating displayed meshes when they are moved
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

// The only change needed here is in InitInstanceBody(), but it and CreateAllInstanceBodies()
// are private in UInstancedStaticMeshComponent, so we end up copypasting them wih copypaste-override 
// of this one
void UPhysInstancedStaticMeshComponent::CreatePhysicsState()
{
//     check(InstanceBodies.Num() == 0);
//
//     FPhysScene* PhysScene = GetWorld()->GetPhysicsScene();
//
//     if (!PhysScene) { return; }
//
// #if WITH_PHYSX
//     //check(Aggregates.Num() == 0);
//
//     const int32 NumBodies = PerInstanceSMData.Num();
//
//     // Aggregates aren't used for static objects
//     const int32 NumAggregates = (Mobility == EComponentMobility::Movable) ? FMath::DivideAndRoundUp<int32>(NumBodies, AggregateMaxSize) : 0;
//
//     // Get the scene type from the main BodyInstance
//     //const uint32 SceneType = BodyInstance.UseAsyncScene() ? PST_Async : PST_Sync;
//
//     for (int32 i = 0; i < NumAggregates; i++)
//     {
//         auto* Aggregate = GPhysXSDK->createAggregate(AggregateMaxSize, false);
//         //Aggregates.Add(Aggregate);
//         //PhysScene->GetPhysXScene(SceneType)->addAggregate(*Aggregate);
//     }
// #endif
//
//     // Create all the bodies.
//     CreateAllInstanceBodies();
//
//     USceneComponent::CreatePhysicsState();
}

void UPhysInstancedStaticMeshComponent::CreateAllInstanceBodies()
{
    int32 NumBodies = PerInstanceSMData.Num();
    //InstanceBodies.Init(NumBodies);

    for (int32 i = 0; i < NumBodies; ++i)
    {
        InstanceBodies[i] = new FBodyInstance;
        InitInstanceBody(i, InstanceBodies[i]);
    }
}

void UPhysInstancedStaticMeshComponent::InitInstanceBody(int32 InstanceIdx, FBodyInstance* InstanceBodyInstance)
{
    // if (!StaticMesh)
    // {
    //     UE_LOG(LogStaticMesh, Warning, TEXT("Unabled to create a body instance for %s in Actor %s. No StaticMesh set."), *GetName(), GetOwner() ? *GetOwner()->GetName() : TEXT("?"));
    //     return;
    // }

    check(InstanceIdx < PerInstanceSMData.Num());
    check(InstanceIdx < InstanceBodies.Num());
    check(InstanceBodyInstance);

    UBodySetup* BodySetup = GetBodySetup();
    check(BodySetup);

    // Get transform of the instance
    //FTransform InstanceTransform = FTransform(PerInstanceSMData[InstanceIdx].Transform) * ComponentToWorld;

    InstanceBodyInstance->CopyBodyInstancePropertiesFrom(&BodyInstance);
    InstanceBodyInstance->InstanceBodyIndex = InstanceIdx; // Set body index 

// original:
    // make sure we never enable bSimulatePhysics for ISMComps
    //InstanceBodyInstance->bSimulatePhysics = false;
// new:
    InstanceBodyInstance->SetInstanceSimulatePhysics(IsSimulatingPhysics());

#if WITH_PHYSX
// original:
    // Create physics body instance.
	// auto* Aggregate = (Mobility == EComponentMobility::Movable) ? Aggregates[FMath::DivideAndRoundDown<int32>(InstanceIdx, AggregateMaxSize)] : nullptr;
	// check(Mobility != EComponentMobility::Movable || Aggregate->getNbActors() < Aggregate->getMaxNbActors());
 //    InstanceBodyInstance->bAutoWeld = false;	//We don't support this for instanced meshes.
	// InstanceBodyInstance->InitBody(BodySetup, InstanceTransform, this, GetWorld()->GetPhysicsScene(), Aggregate);
// new:
 //    auto* Aggregate = (Mobility == EComponentMobility::Movable && !BodyInstance.bSimulatePhysics) ?
 //        Aggregates[FMath::DivideAndRoundDown<int32>(InstanceIdx, AggregateMaxSize)] : nullptr;
	// check(Mobility != EComponentMobility::Movable || Aggregate->getNbActors() < Aggregate->getMaxNbActors());
 //    InstanceBodyInstance->bAutoWeld = false;	//We don't support this for instanced meshes.
	// InstanceBodyInstance->InitBody(BodySetup, InstanceTransform, this, GetWorld()->GetPhysicsScene(), Aggregate);
#endif //WITH_PHYSX
}

// Hackish way to handle impacts. Remember parameters and apply them when when Hit event will 
// be received and we will know exact Instance.
void UPhysInstancedStaticMeshComponent::AddImpulse(FVector Impulse, FName BoneName, bool bVelChange) {
    LastImpulse = Impulse;
}

void UPhysInstancedStaticMeshComponent::AddImpulseAtLocation(FVector Impulse, FVector Location, FName BoneName)
{
    LastImpulse = Impulse;
    LastLocation = Location;
}

void UPhysInstancedStaticMeshComponent::AddForceAtLocation(FVector Force, FVector Location, FName BoneName)
{
    LastForce = Force;
    LastLocation = Location;
}


bool UPhysInstancedStaticMeshComponent::CanEditSimulatePhysics() {
    return true;
}

// Reusing flag from main BodyInstance
void UPhysInstancedStaticMeshComponent::SetSimulatePhysics(bool bSimulate) {
    BodyInstance.bSimulatePhysics = bSimulate;
}

bool UPhysInstancedStaticMeshComponent::IsSimulatingPhysics(FName BoneName) const {

    return BodyInstance.bSimulatePhysics;
}

// Can be optimized to update transform on moving instances only
void UPhysInstancedStaticMeshComponent::ReceiveTick(float DeltaSeconds) {
    
    for (int i = 0; i < InstanceBodies.Num(); ++i) {
        UpdateInstanceTransform(i, InstanceBodies[i]->GetUnrealWorldTransform());
    }
}

// Physics handling
void UPhysInstancedStaticMeshComponent::OnHit(UPrimitiveComponent* HitComp, class AActor* OtherActor,
    class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    FBodyInstance* BI = InstanceBodies[Hit.Item];
    BI->AddImpulse(LastImpulse, false);
    BI->AddImpulseAtPosition(LastImpulse, LastLocation);
    BI->AddForceAtPosition(LastForce, LastLocation);

    LastImpulse = FVector::ZeroVector;
    LastForce = FVector::ZeroVector;
}