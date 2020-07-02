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
#include "Net/UnrealNetwork.h"


// Sets default values
ATPSWeapon::ATPSWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = Cast<USceneComponent>(MeshComp);
	MuzzleSocketName = "MuzzleSocket";
	TracerName = "BeamEnd";
	BaseDamage = 20.0f;

	RateofFire = 500;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateofFire;

	ActualDamage = BaseDamage;
}

void ATPSWeapon::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
	}

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
		EPhysicalSurface SurfaceType = SurfaceType_Default;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			AActor *HitActor = HitResult.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage = BaseDamage * 4;
			}
			PlayImpactEffects(SurfaceType, HitResult.ImpactPoint);
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, HitResult, MyOwner->GetInstigatorController(), this, DamageType);
			TracerEndPoint = HitResult.ImpactPoint;
		}
		//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		PlayFireEffects(TracerEndPoint);
		UE_LOG(LogTemp, Warning, TEXT("TracerEndPoint %d "), TracerEndPoint.Size())
		TracerEndPoint = -1 * TracerEndPoint;

		if (HasAuthority())
		{
			HitScanTracer.TraceTo = -1*TracerEndPoint;
			HitScanTracer.SurfaceType = SurfaceType;
		}

		LastTimeFire = GetWorld()->GetTimeSeconds();
	}
}

void ATPSWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ATPSWeapon::ServerFire_Validate()
{
	return true;
}

void ATPSWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTracer.TraceTo);
	PlayImpactEffects(HitScanTracer.SurfaceType, HitScanTracer.TraceTo);
}

void ATPSWeapon::StartFire()
{
	float DelayBetweenShots = FMath::Max(LastTimeFire + TimeBetweenShots - GetWorld()->GetTimeSeconds(), 0.0f);
	GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &ATPSWeapon::Fire, TimeBetweenShots, true, DelayBetweenShots);
}

void ATPSWeapon::EndFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireRateHandle);
}

void ATPSWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
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
			APawn *MyOwner = Cast<APawn>(GetOwner());
			if (MyOwner)
			{
				APlayerController *PC = Cast<APlayerController>(MyOwner->GetController());
				if (PC)
				{
					PC->ClientPlayCameraShake(FireCameraShake);
				}
			}
		}
	}
}

void ATPSWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem *SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = FleshImpactEffect;
		break;

	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;

	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ATPSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATPSWeapon, HitScanTracer, COND_SkipOwner);
}