#pragma once
#include "GameFramework/Actor.h"
#include "MessSpawner.generated.h"

class AGameObject;
class UBoxComponent;

UCLASS(Blueprintable)
class AMessSpawner : public AActor
{
	GENERATED_BODY()
public:
	AMessSpawner();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGameObject> BP_Subclass;

protected:
	virtual void BeginPlay() override;
 	
private:
	UFUNCTION()
	void spawnMess();

	int counter = 0;
	
	FTimerHandle _timeHandle;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* _spawningArea;
};