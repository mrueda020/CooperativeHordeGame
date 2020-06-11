// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSExplosiveBarrel.generated.h"


class UTPSHealthComponent;
class UStaticMeshComponent;
class UParticleSystem;
class URadialForceComponent;


UCLASS()
class HORDEGAME_API ATPSExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSExplosiveBarrel();

protected:

	UFUNCTION()
	void OnHealtChanged(UTPSHealthComponent* OwningHealtComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTPSHealthComponent* HealtComponent;

	UPROPERTY(VisibleAnywhere,  Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* ExplosionEffect;

	bool bhasExploded;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ExplosionImpulse;

public:	
	

};
