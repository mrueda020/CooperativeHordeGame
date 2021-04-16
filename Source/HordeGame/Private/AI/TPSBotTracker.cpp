// Fill out your copyright notice in the Description page of Project Settings.


#include "HordeGame/Public/AI/TPSBotTracker.h"
#include "HordeGame/Public/Components/TPSHealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "TPSCharacter.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATPSBotTracker::ATPSBotTracker()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCanEverAffectNavigation(false);
	RootComponent = MeshComponent;

	HealthComponent = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPSBotTracker::HandleTakeAnyDamage);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(350);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetupAttachment(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	//AudioComponent->AttachTo(RootComponent);

	ForceMovement = 1000.0f;
	bUseAccelChange = true;
	RequiredDistanceToNextPathPoint = 1000.0f;

	BaseDamage = 45.0f;
	DamageRadius = 250.0f;
	bExploded = false;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce Componet"));
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false;
	RadialForceComponent->Radius = DamageRadius;
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->SetupAttachment(RootComponent);

	ExplosionImpulse = 1000.0f;

	LifeSpanAfterExploded = 1.5f;

	MinDistanceBetweenBots = 560.0f;
	DamageMultiplier = 1;
	SetReplicates(true);
	SetReplicateMovement(true);
}


// Called when the game starts or when spawned
void ATPSBotTracker::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		if (PlayerObjectiveClass)
		{
			TArray<AActor*> ReturnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, PlayerObjectiveClass, ReturnedActors);

			if (ReturnedActors.Num() > 0)
			{
				ActorToFollow = ReturnedActors[0];
			}

			NextPathPoint = GetNextPathPoint();
		}

	}
}

FVector ATPSBotTracker::GetNextPathPoint()
{
	if (ActorToFollow)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), GetActorLocation(), ActorToFollow);
		if (NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}

	}

	return GetActorLocation();
}

void ATPSBotTracker::ChaseActor()
{
	if (ActorToFollow)
	{
		float DistanceToNextPathPoint = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToNextPathPoint <= RequiredDistanceToNextPathPoint)
		{
			NextPathPoint = GetNextPathPoint();
		}

		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= ForceMovement;
			MeshComponent->AddForce(ForceDirection, NAME_None, bUseAccelChange);
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Black, false, 0.0f, 0);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 35.0f, 32, FColor::Red, false, 1.0f);

	}
}

void ATPSBotTracker::HandleTakeAnyDamage(UTPSHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{


	if (MaterialInstance == nullptr)
	{
		MaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	}

	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->GetTimeSeconds());
	}

	if (Health <= 0.0f && !bExploded)
	{
		Explode();
	}

}


void ATPSBotTracker::Explode()
{	
	
	bExploded = true;
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	if (HasAuthority())
	{
		TArray<AActor*> IgnoredActors = { this };		
		float ActualDamage = BaseDamage;
		ActualDamage *= DamageMultiplier;
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ActualDamage, GetActorLocation(), DamageRadius, nullptr, IgnoredActors, this, this->GetInstigatorController(), true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 32, FColor::Green, false, 1.0f);
		FVector ImpulseVector = FVector::ForwardVector * ExplosionImpulse;
		MeshComponent->AddImpulse(ImpulseVector, NAME_None, true);
		RadialForceComponent->FireImpulse();
		SetLifeSpan(LifeSpanAfterExploded);
	}
	
}


void ATPSBotTracker::InflictSelfDamage()
{
	UGameplayStatics::ApplyDamage(this, 15, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ATPSBotTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{	
		
		//PlayRollingSound();
		ChaseActor();
		CheckNearBots();
	}
}

void ATPSBotTracker::PlayRollingSound()
{
	float Velocity = GetVelocity().Size();
	float AudioMultiplier = MapRangedClamped(Velocity, 10, 1000, 1, 3);
	//UE_LOG(LogTemp, Warning, TEXT("Velocity : %f  ,  Volume %f"), Velocity, AudioMultiplier);
	AudioComponent->SetVolumeMultiplier(AudioMultiplier);
}

void ATPSBotTracker::CheckNearBots()
{
	float PowerLevel = 1;
	TArray<AActor*> ReturnedActors;
	UGameplayStatics::GetAllActorsOfClass(this, this->GetClass(), ReturnedActors);
	if (ReturnedActors.Num() > 1)
	{
		for (int32 i = 0; i < ReturnedActors.Num(); i++)
		{
			float Distance = (GetActorLocation() - ReturnedActors[i]->GetActorLocation()).Size();

			if (Distance < MinDistanceBetweenBots && Distance != 0)
			{
				PowerLevel += 1;
			}
			else
			{
				PowerLevel = PowerLevel <= 0 ? 1 : PowerLevel--;
			}


		}

	}

	const int32 MaxPowerLevel = 4;

	DamageMultiplier = PowerLevel < 1 ? 1 : PowerLevel < MaxPowerLevel ? PowerLevel : MaxPowerLevel;
	
	Alpha = PowerLevel == 1 ? 0 : PowerLevel / float(MaxPowerLevel);


	if(HasAuthority())
	{
		
		if (MaterialInstance == nullptr)
		{
			MaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
		}

		if (MaterialInstance)
		{
			MaterialInstance->SetScalarParameterValue("PowerLevelAlpha", Alpha);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Damage %f"), DamageMultiplier)
}


void ATPSBotTracker::NotifyActorBeginOverlap(AActor* OtherActor)
{	
	if(bExploded)
	{
		return;
	}
	if (!bStartedSelfDestruction)
	{
		ATPSCharacter* PlayerPawn = Cast<ATPSCharacter>(OtherActor);
		if (PlayerPawn)
		{	
			if (HasAuthority())
			{
				GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ATPSBotTracker::InflictSelfDamage, 0.05f, true, 0.0f);
			}
			bStartedSelfDestruction = true;
			UGameplayStatics::SpawnSoundAttached(TriggerExplosionSound, RootComponent);
		}
	}
}

float ATPSBotTracker::MapRangedClamped(float value, float inRangeA, float inRangeB, float outRangeA, float outRangeB)
{
	if (outRangeA == outRangeB) return outRangeA;
	if (inRangeA == inRangeB) UE_LOG(LogTemp, Warning, TEXT("inRangeA == inRangeB which will produce one to many mapping"));
	float inPercentage = (value - inRangeA) / (inRangeB - inRangeA);
	if (inPercentage < 0.0f) return outRangeA;
	if (inPercentage > 1.0f) return outRangeB;
	return outRangeA + inPercentage * (outRangeB - outRangeA);
}

void ATPSBotTracker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSBotTracker, Alpha);
}