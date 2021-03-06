// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSExplosiveBarrel.h"
#include "HordeGame/Public/Components/TPSHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HordeGame/HordeGame.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATPSExplosiveBarrel::ATPSExplosiveBarrel()
{
	ExplosionRadius = 500.0f;
	ExplosionImpulse = 500.0f;
	BaseDamage = 150.0f;

	HealtComponent = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health Component"));
	//Syncronise the TPSHealthComponent function HandleTakeAnyDamage with OnHealthChanged
	HealtComponent->OnHealthChanged.AddDynamic(this, &ATPSExplosiveBarrel::OnHealthChanged);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

	RootComponent = MeshComp;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce Componet"));
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->Radius = ExplosionRadius;
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->SetupAttachment(RootComponent);

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ATPSExplosiveBarrel::OnHealthChanged(UTPSHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bhasExploded)
	{
		
		bhasExploded = true; //Replicate
		OnRep_Explode();

		FVector UpwardVector = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(UpwardVector, NAME_None, true);
		RadialForceComponent->FireImpulse();

		TArray<AActor*> IgnoredActors = {this};
		UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, this->GetInstigatorController(), false, COLLISION_WEAPON);
	}
}

void ATPSExplosiveBarrel::OnRep_Explode()
{
	//We play cosmetic effects
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	DrawDebugSphere(GetWorld(), this->GetActorLocation(), ExplosionRadius, 32, FColor::Red, false, 1.5f);
}

void ATPSExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATPSExplosiveBarrel, bhasExploded);
}
