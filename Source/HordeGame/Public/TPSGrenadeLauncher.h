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

    /** Projectile class to spawn */
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<ATPSGrenade> ProjectileClass;
  
};
