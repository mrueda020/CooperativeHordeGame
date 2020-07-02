// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSGrenade.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "HordeGame/HordeGame.h"

// Sets default values
ATPSGrenade::ATPSGrenade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 1 seconds by default
	InitialLifeSpan = 1.0f;
	MaxFuzeTime = 0.5f;
	BaseDamage = 100.0f;
	DamageRadius = 10.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATPSGrenade::BeginPlay()
{
	Super::BeginPlay();

	if (ExplosionEffect)
	{
		GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ATPSGrenade::Explode, MaxFuzeTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("It should explode"))
	}
}

void ATPSGrenade::Explode()
{
	UE_LOG(LogTemp, Warning, TEXT("Explosion effect"));
	TArray<AActor *> IgnoredActors = {this};
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, this->GetActorLocation());
	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRadius, DamageType, IgnoredActors, this, this->GetInstigatorController(),false, COLLISION_WEAPON);
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), DamageRadius, 32, FColor::Black, false, 1.5f);
	Destroy();
}

// Called every frame
void ATPSGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
