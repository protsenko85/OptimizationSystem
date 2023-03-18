#include "GameObject.h"

#include "Kismet/GameplayStatics.h"
#include "tools/InstancedStaticMeshesHolder.h"

class AInstancedStaticMeshesHolder;

AGameObject::AGameObject()
{
	_meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	_meshComponent->SetupAttachment(RootComponent);
}

UStaticMeshComponent* AGameObject::getMeshComponent() const
{
	return _meshComponent;
}

void AGameObject::BeginPlay()
{
	Super::BeginPlay();
	
	if (_meshComponent->IsSimulatingPhysics())
	{
		TArray<AActor*> holders;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInstancedStaticMeshesHolder::StaticClass(), holders);
	    Cast<AInstancedStaticMeshesHolder>(holders[0])->addNewMeshToInstancedStaticMesh(this, 0);
	}
}
