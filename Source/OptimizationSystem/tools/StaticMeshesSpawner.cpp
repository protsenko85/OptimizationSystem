#include "StaticMeshesSpawner.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OptimizationSystem/GameObject.h"

AStaticMeshesSpawner::AStaticMeshesSpawner()
{
    InstancedStaticMeshComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(
        FName("HierarchicalInstancedStaticMeshComponent"));
    InstancedStaticMeshComponent->bMultiBodyOverlap = true;
    InstancedStaticMeshComponent->bHasPerInstanceHitProxies = true;
    InstancedStaticMeshComponent->NumCustomDataFloats = 3;
    InstancedStaticMeshComponent->SetNotifyRigidBodyCollision(true);
    
    SetRootComponent(InstancedStaticMeshComponent);
    _instancedStaticMeshesActors = new TArray<TPair<AActor*, FBodyInstance*>>();
}

void AStaticMeshesSpawner::createMeshInstanceFromObject(AActor* copiedObject)
{
    deactivateObject(Cast<AGameObject>(copiedObject));
    
    const FTransform& objectTransform = copiedObject->GetActorTransform();
    const int32 spawnedInstanceIndex = InstancedStaticMeshComponent->AddInstanceWorldSpace(objectTransform);

    if (InstancedStaticMeshComponent->IsSimulatingPhysics())
    {
        UE_LOG(LogTemp, Log, TEXT("Simulating!"));
    }

    // TPair<AActor*, FBodyInstance*>* foundData =
    //     _instancedStaticMeshesActors->FindByPredicate([copiedObject](TPair<AActor*, FBodyInstance*> data)
    // {
    //     return copiedObject == data.Key;
    // });
    //
    // if (foundData != nullptr)
    // {
    //     foundData->Value = InstancedStaticMeshComponent->InstanceBodies[spawnedInstanceIndex];
    // }
    // else
    // {
    //     const TPair<AActor*, FBodyInstance*> newPair(copiedObject,
    //         InstancedStaticMeshComponent->InstanceBodies[spawnedInstanceIndex]);
    //     _instancedStaticMeshesActors->Add(newPair);
    // }

    copiedObject->OnDestroyed.AddUniqueDynamic(this, &AStaticMeshesSpawner::OnHeldObjectDestroyed);
}

AGameObject* AStaticMeshesSpawner::createObjectFromMeshInstance(int meshInstanceIndex)
{    
    FTransform meshTransform;
    const bool removedInstance = InstancedStaticMeshComponent->GetInstanceTransform(meshInstanceIndex, meshTransform, true);
    if (removedInstance)
    {
        TPair<AActor*, FBodyInstance*>* heldObjectInfo = getHeldMeshDataByIndex(meshInstanceIndex);

        if (heldObjectInfo != nullptr)
        {
            heldObjectInfo->Value = nullptr;
            InstancedStaticMeshComponent->RemoveInstance(meshInstanceIndex);
            AGameObject* keyGameObject = Cast<AGameObject>(heldObjectInfo->Key);
            activateObject(keyGameObject);
            return keyGameObject;
        }
    }

    return nullptr;
}

void AStaticMeshesSpawner::init(UStaticMesh* exampleMesh)
{
    _exampleMesh = exampleMesh;
}

void AStaticMeshesSpawner::addConvertableObject(AGameObject* addedObject)
{
    // const TPair<AActor*, FBodyInstance*>* heldObjectInfo = getHeldMeshDataByObject(addedObject);
    //
    // if (heldObjectInfo == nullptr)
    // {
    //     const TPair<AActor*, FBodyInstance*> newPair(addedObject, nullptr);
    //     _instancedStaticMeshesActors->Add(newPair);
    // }

    //changeAllActorsToMeshes();
    
    createMeshInstanceFromObject(addedObject);
}

void AStaticMeshesSpawner::removeInstanceMeshInfoByObject(AGameObject* removedObject)
{
    const int removedInstanceIndex = _instancedStaticMeshesActors->FindLastByPredicate([removedObject](TPair<AActor*, FBodyInstance*> data)
    {
        return removedObject == data.Key;
    });

    removeInstanceMeshInfoByIndex(removedInstanceIndex);
}

void AStaticMeshesSpawner::removeInstanceMeshInfoByIndex(int meshInstanceIndex) const
{
    if (meshInstanceIndex != INDEX_NONE)
    {
        _instancedStaticMeshesActors->RemoveAt(meshInstanceIndex);
    }
}

void AStaticMeshesSpawner::BeginPlay()
{
    Super::BeginPlay();

    _instancedStaticMeshesActors->Empty();

    InstancedStaticMeshComponent->SetStaticMesh(_exampleMesh);
    
    InstancedStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
    InstancedStaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AStaticMeshesSpawner::OnStaticMeshBeginOverlap);
    InstancedStaticMeshComponent->OnComponentHit.AddDynamic(this, &AStaticMeshesSpawner::OnStaticMeshHit);

    //enableMeshCreatingTimer();
}

void AStaticMeshesSpawner::BeginDestroy()
{
    delete _instancedStaticMeshesActors;
    _instancedStaticMeshesActors = nullptr;

    Super::BeginDestroy();
}

void AStaticMeshesSpawner::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    //changeAllActorsToMeshes();
}

void AStaticMeshesSpawner::activateObject(AGameObject* objectToEnable)
{
    objectToEnable->SetActorTickEnabled(true);
    objectToEnable->SetActorHiddenInGame(false);
    objectToEnable->SetActorEnableCollision(true);

    UMeshComponent* meshComponent = objectToEnable->getMeshComponent();
    meshComponent->SetSimulatePhysics(true);
    meshComponent->SetNotifyRigidBodyCollision(true);
}

void AStaticMeshesSpawner::deactivateObject(AGameObject* objectToDisable)
{
    objectToDisable->SetActorTickEnabled(false);
    objectToDisable->SetActorHiddenInGame(true);
    objectToDisable->SetActorEnableCollision(false);
    objectToDisable->getMeshComponent()->SetSimulatePhysics(false);
}

void AStaticMeshesSpawner::enableMeshCreatingTimer()
{
    if (!MeshCreatingTimer.IsValid())
    {
        GetWorldTimerManager().SetTimer(MeshCreatingTimer, this, &AStaticMeshesSpawner::changeAllActorsToMeshes,
            3.f, true);
    }
}

void AStaticMeshesSpawner::disableMeshCreatingTimer()
{
    GetWorldTimerManager().SetTimer(RefreshingTimer, this, &AStaticMeshesSpawner::enableMeshCreatingTimer,
        3.f, false);
    if (MeshCreatingTimer.IsValid())
    {
        GetWorldTimerManager().ClearTimer(MeshCreatingTimer);
        changeAllMeshesToActors();
    }
}

void AStaticMeshesSpawner::changeAllMeshesToActors()
{
    InstancedStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
    for (const TPair<AActor*, FBodyInstance*>& pair : *_instancedStaticMeshesActors)
    {
        if (pair.Key != nullptr && pair.Key->IsHidden())
        {
            pair.Key->SetActorTickEnabled(true);
            pair.Key->SetActorHiddenInGame(false);
            pair.Key->SetActorEnableCollision(true);
        }
    }

    InstancedStaticMeshComponent->ClearInstances();
    InstancedStaticMeshComponent->SetCollisionProfileName("Interactable");

    for (TPair<AActor*, FBodyInstance*>& pair : *_instancedStaticMeshesActors)
    {
        UMeshComponent* meshComponent = Cast<AGameObject>(pair.Key)->getMeshComponent();
        if (!meshComponent->IsSimulatingPhysics())
        {
            meshComponent->SetSimulatePhysics(true);
            meshComponent->SetNotifyRigidBodyCollision(true);
        }
    }
}

void AStaticMeshesSpawner::changeAllActorsToMeshes()
{
    for (TPair<AActor*, FBodyInstance*>& pair : *_instancedStaticMeshesActors)
    {
        if (isActorPreparedForChangeToStaticMesh(Cast<AGameObject>(pair.Key)))
        {
            createMeshInstanceFromObject(pair.Key);
        }
    }
}

TPair<AActor*, FBodyInstance*>* AStaticMeshesSpawner::getHeldMeshDataByIndex(int meshInstanceIndex) const
{
    const FBodyInstance* bodyInstance = InstancedStaticMeshComponent->InstanceBodies[meshInstanceIndex];
    TPair<AActor*, FBodyInstance*>* foundData =
        _instancedStaticMeshesActors->FindByPredicate([bodyInstance](TPair<AActor*, FBodyInstance*> data)
    {
        return bodyInstance == data.Value;
    });

    return foundData;
}

TPair<AActor*, FBodyInstance*>* AStaticMeshesSpawner::getHeldMeshDataByObject(AGameObject* heldObject) const
{
    TPair<AActor*, FBodyInstance*>* foundData =
        _instancedStaticMeshesActors->FindByPredicate([heldObject](TPair<AActor*, FBodyInstance*> data)
    {
        return heldObject == data.Key;
    });

    return foundData;
}

bool AStaticMeshesSpawner::isActorPreparedForChangeToStaticMesh(AGameObject* actor)
{
    return !actor->IsHidden()
            && actor->GetAttachParentActor() == nullptr
            && actor->GetVelocity().Size() < 1.f
            && actor->getMeshComponent()->GetCollisionObjectType() == ECC_GameTraceChannel1;
}

void AStaticMeshesSpawner::OnStaticMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    disableMeshCreatingTimer();
}

void AStaticMeshesSpawner::OnStaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (SweepResult.bBlockingHit || OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel7)
    {
        return;
    }
    if (OtherBodyIndex == INDEX_NONE)
    {
        createObjectFromMeshInstance(SweepResult.Item);
    }
}

void AStaticMeshesSpawner::OnHeldObjectDestroyed(AActor* DestroyedActor)
{
    removeInstanceMeshInfoByObject(Cast<AGameObject>(DestroyedActor));
}
