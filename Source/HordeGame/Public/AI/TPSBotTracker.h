// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPSBotTracker.generated.h"

class UStaticMeshComponent;
class UTPSHealthComponent;
class UMaterialInstanceDynamic;
class USphereComponent;
class URadialForceComponent;
class USoundCue;
class UAudioComponent;

UCLASS()
class HORDEGAME_API ATPSBotTracker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATPSBotTracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UTPSHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	TSubclassOf<AActor> PlayerObjectiveClass;

	AActor* ActorToFollow;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	float ForceMovement;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	float RequiredDistanceToNextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	bool bUseAccelChange;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	float LifeSpanAfterExploded;

	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	USoundCue* ExplosionSound;


	UPROPERTY(EditDefaultsOnly, Category = "BotTracker")
	USoundCue* TriggerExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	UParticleSystem* ExplosionEffect;

	void Explode();

	FVector NextPathPoint;

	FVector GetNextPathPoint();

	void ChaseActor();

	UFUNCTION()
		void HandleTakeAnyDamage(UTPSHealthComponent* OwningHealthComp, float Health, float HealthDelta,
			const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FTimerHandle FuzeTimerHandle;

	bool bStartedSelfDestruction;

	void InflictSelfDamage();

	float MapRangedClamped(float value, float inRangeA, float inRangeB, float outRangeA, float outRangeB);

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float MinDistanceBetweenBots;

	void CheckNearBots();

	void PlayRollingSound();

	float DamageMultiplier;
	
	UPROPERTY(Replicated)
	float Alpha;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/**
 *	Event when this actor overlaps another actor, for example a player walking into a trigger.
 *	For events when objects have a blocking collision, for example a player hitting a wall, see 'Hit' events.
 *	@note Components on both this and the other Actor must have bGenerateOverlapEvents set to true to generate overlap events.
 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
