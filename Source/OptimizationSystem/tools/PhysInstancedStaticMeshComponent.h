#pragma once
#include "Components/InstancedStaticMeshComponent.h"
#include "PhysInstancedStaticMeshComponent.generated.h"


UCLASS()
class UPhysInstancedStaticMeshComponent : public UInstancedStaticMeshComponent
{
    GENERATED_UCLASS_BODY()

    UPROPERTY()
    FVector LastImpulse;

    UPROPERTY()
    FVector LastForce;

    UPROPERTY()
    FVector LastLocation;

public:
    virtual void CreatePhysicsState();// override;

    virtual void AddImpulse(FVector Impulse, FName BoneName = NAME_None, bool bVelChange = false) override;
    virtual void AddImpulseAtLocation(FVector Impulse, FVector Location, FName BoneName = NAME_None) override;
    virtual void AddForceAtLocation(FVector Force, FVector Location, FName BoneName = NAME_None) override;

    virtual bool CanEditSimulatePhysics() override;

    virtual void SetSimulatePhysics(bool bSimulate) override;
    virtual bool IsSimulatingPhysics(FName BoneName = NAME_None) const override;

    virtual void ReceiveTick(float DeltaSeconds);// override;
private:
    void InitInstanceBody(int32 InstanceIdx, FBodyInstance* BodyInstance);
    void CreateAllInstanceBodies();

    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
