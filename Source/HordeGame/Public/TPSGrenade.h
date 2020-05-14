// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSGrenade.generated.h"

class UProjectileMovementComponent;
class USphereComponent;


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
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	FTimerHandle FuzeTimerHandle;

	float MaxFuzeTime;

	void Explode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
