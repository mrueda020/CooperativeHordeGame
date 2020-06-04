// Fill out your copyright notice in the Description page of Project Settings.


#include "HordeGame/Public/Components/TPSHealthComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	
	DefaultHealth = 100.0f;

}


// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this,&UTPSHealthComponent::HandleTakeAnyDamage);
	}

	Health = DefaultHealth;

}

void UTPSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
	{
		return;
	}

	// Update Health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("Damage: %s, Health: %s"), *FString::SanitizeFloat(Damage), *FString::SanitizeFloat(Health));

}

