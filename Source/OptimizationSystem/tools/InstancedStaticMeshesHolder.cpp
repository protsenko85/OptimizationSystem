#include "InstancedStaticMeshesHolder.h"
#include "StaticMeshesSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "OptimizationSystem/GameObject.h"

AInstancedStaticMeshesHolder::AInstancedStaticMeshesHolder()
{
}

void AInstancedStaticMeshesHolder::addNewMeshToInstancedStaticMesh(AGameObject* addedObject, int optimizingIndex)
{
    AStaticMeshesSpawner* staticMeshesSpawner = findStaticMeshesSpawnerByObjectType(optimizingIndex);
    
    if (staticMeshesSpawner == nullptr)
    {
        staticMeshesSpawner = GetWorld()->SpawnActorDeferred<AStaticMeshesSpawner>(AStaticMeshesSpawner::StaticClass(), FTransform());
        staticMeshesSpawner->init(addedObject->getMeshComponent()->GetStaticMesh());
        UGameplayStatics::FinishSpawningActor(staticMeshesSpawner, FTransform());
        _instancedStaticMeshesInfo.Add(optimizingIndex, staticMeshesSpawner);
    }

    staticMeshesSpawner->addConvertableObject(addedObject);
}

void AInstancedStaticMeshesHolder::removeObjectFromInstancedStaticMeshes(AGameObject* removedObject, int optimizingIndex) const
{
    AStaticMeshesSpawner* staticMeshesSpawner = findStaticMeshesSpawnerByObjectType(optimizingIndex);
    
    if (staticMeshesSpawner != nullptr)
    {
        staticMeshesSpawner->removeInstanceMeshInfoByObject(removedObject);
    }
}

void AInstancedStaticMeshesHolder::BeginDestroy()
{
    Super::BeginDestroy();
    
    TArray<UActorComponent*> holdingComponents = GetInstanceComponents();
    for (UActorComponent* component : holdingComponents)
    {
        component->DestroyComponent();
    }
}

AStaticMeshesSpawner* AInstancedStaticMeshesHolder::findStaticMeshesSpawnerByObjectType(int objectType) const
{
    if (_instancedStaticMeshesInfo.Num() > 0)
    {
        return _instancedStaticMeshesInfo.FindRef(objectType);
    }

    return nullptr;
}
