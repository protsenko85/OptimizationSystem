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
	GetWorldTimerManager().SetTimer(_timeHandle, this, &AMessSpawner::spawnMess, 0.01f, true, 0.f);
	
    // const float mixX = _spawningArea->GetComponentLocation().X - _spawningArea->GetLocalBounds().BoxExtent.X;
    // const float maxX = _spawningArea->GetComponentLocation().X + _spawningArea->GetLocalBounds().BoxExtent.X;
    // const float mixY = _spawningArea->GetComponentLocation().Y - _spawningArea->GetLocalBounds().BoxExtent.Y;
    // const float maxY = _spawningArea->GetComponentLocation().Y + _spawningArea->GetLocalBounds().BoxExtent.Y;
    // const float z = _spawningArea->GetComponentLocation().Y;
    // for (int x = mixX; x < maxX; x++)
    // {
    //     for( int y = mixY; y < maxY; y++)
    //     {
    //         //FVector location = FVector(x, y, z);
    //         FVector location = GetActorLocation();
    //         AGameObject* spawningObject = GetWorld()->SpawnActorDeferred<AGameObject>(BP_Subclass,
    //             FTransform(location));
    //         UGameplayStatics::FinishSpawningActor(spawningObject, FTransform(location));
    //     }
    // }
}

void AMessSpawner::spawnMess()
{
	const FVector location = GetActorLocation();
	AGameObject* spawningObject = GetWorld()->SpawnActorDeferred<AGameObject>(BP_Subclass,
				FTransform(location));
	UGameplayStatics::FinishSpawningActor(spawningObject, FTransform(location));
	
	counter++;
	FString str = FString::FromInt(counter);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, str);	
}
