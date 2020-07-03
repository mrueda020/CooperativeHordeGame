// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSGrenade.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UDamageType;


UCLASS()
class HORDEGAME_API ATPSGrenade : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATPSGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent *ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem *ExplosionEffect;

	FTimerHandle FuzeTimerHandle;

	float MaxFuzeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	void Explode();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayExplosionEffects(FVector Location);
};
