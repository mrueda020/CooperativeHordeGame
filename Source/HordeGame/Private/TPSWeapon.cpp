// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "HordeGame/HordeGame.h"
#include "TimerManager.h"

// Sets default values
ATPSWeapon::ATPSWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = Cast<USceneComponent>(MeshComp);
	MuzzleSocketName = "MuzzleSocket";
	TracerName = "BeamEnd";
	BaseDamage = 20.0f;

	RateofFire = 500;
}

void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateofFire;
}

void ATPSWeapon::Fire()
{
	AActor *MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotator;
		//this would define the eye height location, and view rotation (which is different from the pawn rotation which has a zeroed pitch component)
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotator);
		FVector ShotDirection = EyeRotator.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
		FVector TracerEndPoint = TraceEnd;
		//Structure that defines parameters passed into collision function
		FCollisionQueryParams QueryParams;
		TArray<AActor *> IgnoredActors = {MyOwner, this};
		QueryParams.AddIgnoredActors(IgnoredActors);
		QueryParams.bReturnPhysicalMaterial = true;
		//Whether we should trace against complex collision
		QueryParams.bTraceComplex = true;
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			TracerEndPoint = HitResult.ImpactPoint;
			AActor *HitActor = HitResult.GetActor();
			

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
			UParticleSystem* SelectedEffect = nullptr;
			float ActualDamage = BaseDamage;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
				SelectedEffect = FleshImpactEffect;
				break;

			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				ActualDamage *= 4.0f;
				break;
			
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, HitResult, MyOwner->GetInstigatorController(), this, DamageType);

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
			
		}
		//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		PlayFireEffects(TracerEndPoint);

		LastTimeFire = GetWorld()->GetTimeSeconds();
	}
}

void ATPSWeapon::StartFire()
{	
	float DelayBetweenShots = FMath::Max(LastTimeFire + TimeBetweenShots - GetWorld()->GetTimeSeconds(),0.0f);
	GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &ATPSWeapon::Fire,TimeBetweenShots , true, DelayBetweenShots);
}

void ATPSWeapon::EndFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateHandle);
}

void ATPSWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, RootComponent, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent *TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(TracerName, TracerEndPoint);
		}

		if (FireCameraShake)
		{
			APawn* MyOwner = Cast<APawn>(GetOwner());
			if (MyOwner)
			{
				APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
				if (PC)
				{
					PC->ClientPlayCameraShake(FireCameraShake);
				}
			}
		}
	}
}
