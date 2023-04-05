#include "MessSpawner.h"

#include <string>

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "OptimizationSystem/GameObject.h"
#include "iostream"

AMessSpawner::AMessSpawner()
{
    _spawningArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectComponent"));
    _spawningArea->SetupAttachment(RootComponent);
}

void AMessSpawner::Tick(float DeltaSeconds)
{
}

void AMessSpawner::BeginPlay()
{
	for(int i = 0; i < 10000; i++)
	{
		spawnMess();
	}
	// GetWorldTimerManager().SetTimer(_timeHandle, this, &AMessSpawner::spawnMess, 0.05f, true, 0.f);
}

void AMessSpawner::spawnMess()
{
	const FVector location = GetActorLocation() + FMath::VRand() * 5000;
	AGameObject* spawningObject = GetWorld()->SpawnActorDeferred<AGameObject>(BP_Subclass,
				FTransform(location));
	UGameplayStatics::FinishSpawningActor(spawningObject, FTransform(location));
}
