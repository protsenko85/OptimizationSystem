#include "GameObject.h"

#include "Kismet/GameplayStatics.h"
#include "tools/InstancedStaticMeshesHolder.h"
#include "tools/StaticMeshesSpawner.h"

class AInstancedStaticMeshesHolder;

AGameObject::AGameObject() : _optimizationEnabled(false)
{
	_meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	_meshComponent->SetupAttachment(RootComponent);
	_meshComponent->SetSimulatePhysics(false);
}

UStaticMeshComponent* AGameObject::getMeshComponent() const
{
	return _meshComponent;
}

void AGameObject::BeginPlay()
{
	Super::BeginPlay();
	
	if (_optimizationEnabled)
	{
		TArray<AActor*> holders;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInstancedStaticMeshesHolder::StaticClass(), holders);
	    Cast<AInstancedStaticMeshesHolder>(holders[0])->addNewMeshToInstancedStaticMesh(this, 0);
	}

	FOnTimelineFloat FallingProgress;
	FOnTimelineEvent FallingFinished;
	FallingProgress.BindUFunction(this, FName("FallingProgress"));
	FallingFinished.BindUFunction(this, FName("FallingFinished"));
	FallingTimeLine.AddInterpFloat(FallingProcessCurve, FallingProgress,
							   NAME_None, NAME_None);
	FallingTimeLine.SetTimelineFinishedFunc(FallingFinished);
	FallingTimeLine.SetLooping(false);

	StartFallLocation = GetActorLocation();
	EndFallLocation = StartFallLocation;
	EndFallLocation.Z = -5000;
	EndFallLocation.X += 20000;

	//FallingTimeLine.PlayFromStart();
}

void AGameObject::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	FallingTimeLine.TickTimeline(DeltaTime);
}

void AGameObject::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FallingTimeLine.TickTimeline(DeltaSeconds);
}

void AGameObject::setInstanceIndex(int index)
{
	_instanceIndex = index;
}

void AGameObject::FallingProgress(float value)
{
	FVector NewLocation;
	NewLocation.X = FMath::Lerp(StartFallLocation.X, EndFallLocation.X, value);
	NewLocation.Y = FMath::Lerp(StartFallLocation.Y, EndFallLocation.Y, value);
	NewLocation.Z = FMath::Lerp(StartFallLocation.Z, EndFallLocation.Z, value);

	UE_LOG(LogTemp, Log, TEXT("Value: %d"), value);

	if (_optimizationEnabled && value > 0)
	{
		SetActorLocation(NewLocation);
		TArray<AActor*> holders;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInstancedStaticMeshesHolder::StaticClass(), holders);
		Cast<AInstancedStaticMeshesHolder>(holders[0])->findStaticMeshesSpawnerByObjectType(0)->
			InstancedStaticMeshComponent->UpdateInstanceTransform(_instanceIndex, FTransform(NewLocation), true);
	}
	SetActorLocation(NewLocation);
}

void AGameObject::FallingFinished()
{
	if (_optimizationEnabled)
	{
		TArray<AActor*> holders;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInstancedStaticMeshesHolder::StaticClass(), holders);
		Cast<AInstancedStaticMeshesHolder>(holders[0])->removeObjectFromInstancedStaticMeshes(this, 0);
	}
	Destroy();
}
