#pragma once

#include "GameFramework/Actor.h"
#include "InstancedStaticMeshesHolder.generated.h"

class AGameObject;
class AStaticMeshesSpawner;

UCLASS()
class AInstancedStaticMeshesHolder : public AActor
{
	GENERATED_BODY()
	
public:
	AInstancedStaticMeshesHolder();
	void addNewMeshToInstancedStaticMesh(AGameObject* addedObject, int optimizingIndex);
	void removeObjectFromInstancedStaticMeshes(AGameObject* removedObject, int optimizingIndex) const;

	virtual void BeginDestroy() override;
	AStaticMeshesSpawner* findStaticMeshesSpawnerByObjectType(int objectType) const;
	
private:
	UPROPERTY()
	TMap<int, AStaticMeshesSpawner*> _instancedStaticMeshesInfo;
};
