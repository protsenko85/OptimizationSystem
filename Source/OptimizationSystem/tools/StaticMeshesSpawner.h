#pragma once

#include "CoreMinimal.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "StaticMeshesSpawner.generated.h"

class AGameObject;
UENUM()
enum EMaterialParameters
{
	DISSOLVE_APPEARANCE = 0,
	DISSOLVE_DESTROY    = 1,
	FLAMMABLE_EFFECT    = 2,
	HEATABLE_EFFECT     = 3
};

UCLASS()
class AStaticMeshesSpawner : public AActor
{
	GENERATED_BODY()

public:
	AStaticMeshesSpawner();
	void createMeshInstanceFromObject(AActor* copiedObject);
	AGameObject* createObjectFromMeshInstance(int meshInstanceIndex);

	void init(UStaticMesh* exampleMesh);

	void addConvertableObject(AGameObject* addedObject);
	void removeInstanceMeshInfoByIndex(int meshInstanceIndex) const;
	void removeInstanceMeshInfoByObject(AGameObject* removedObject);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Settings")
	UHierarchicalInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;
	
private:
	void activateObject(AGameObject* objectToEnable);
	void deactivateObject(AGameObject* objectToDisable);

	void enableMeshCreatingTimer();
	void disableMeshCreatingTimer();
	void changeAllMeshesToActors();
	void changeAllActorsToMeshes();

	TPair<AActor*, FBodyInstance*>* getHeldMeshDataByIndex(int meshInstanceIndex) const;
	TPair<AActor*, FBodyInstance*>* getHeldMeshDataByObject(AGameObject* heldObject) const;

	bool isActorPreparedForChangeToStaticMesh(AGameObject* actor);

	UFUNCTION()
	void OnStaticMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnStaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnHeldObjectDestroyed(AActor* DestroyedActor);

	FTimerHandle MeshCreatingTimer;
	FTimerHandle RefreshingTimer;

	TArray<TPair<AActor*, FBodyInstance*>>* _instancedStaticMeshesActors;

	UStaticMesh* _exampleMesh;
};
