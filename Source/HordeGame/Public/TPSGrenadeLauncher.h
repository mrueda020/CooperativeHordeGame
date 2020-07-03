// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSWeapon.h"
#include "TPSGrenadeLauncher.generated.h"

/**
 * 
 */
class ATPSGrenade;

UCLASS()
class HORDEGAME_API ATPSGrenadeLauncher : public ATPSWeapon
{
	GENERATED_BODY()

protected:

    virtual void Fire() override;

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerFireGrenade();

    UFUNCTION()
    void OnRep_TraceFrom();
 
    UPROPERTY(ReplicatedUsing = OnRep_TraceFrom)
    FVector_NetQuantize TraceFrom;

	/** Projectile class to spawn */
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<ATPSGrenade> ProjectileClass;
	
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

    virtual void StartFire() override;

    virtual void EndFire() override;
 
};
