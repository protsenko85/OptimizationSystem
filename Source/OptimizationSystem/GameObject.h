#pragma once
#include "GameFramework/Actor.h"
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
	
protected:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* _meshComponent;
};
