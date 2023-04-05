#pragma once
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "GameObject.generated.h"

class AInstancedStaticMeshesHolder;

UCLASS(Blueprintable)
class AGameObject : public AActor
{
	GENERATED_BODY()
public:
	AGameObject();
	
	UStaticMeshComponent* getMeshComponent() const;

	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual void Tick(float DeltaSeconds) override;

	void setInstanceIndex(int index);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Take from letter settings")
	UCurveFloat* FallingProcessCurve;
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* _meshComponent;

private:
	FTimeline FallingTimeLine;
	
	UFUNCTION()
	void FallingProgress(float value);
	UFUNCTION()
	void FallingFinished();

	FVector StartFallLocation;
	FVector EndFallLocation;

	bool _optimizationEnabled;

	int _instanceIndex;
};
