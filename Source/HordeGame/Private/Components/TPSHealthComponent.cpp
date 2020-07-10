// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeGame/Public/Components/TPSHealthComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	DefaultHealth = 100.0f;
	SetIsReplicated(true);
}

// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	//We only hook HealtComponent to the server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::HandleTakeAnyDamage);
		}

	}
	Health = DefaultHealth;
}

void UTPSHealthComponent::OnRep_Health(float PreviousHealth)
{
	float Damage = Health - PreviousHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UTPSHealthComponent::HandleTakeAnyDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType, AController *InstigatedBy, AActor *DamageCauser)
{
	
	if (Damage <= 0)
	{
		return;
	}

	// Update Health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("Damage: %s, Health: %s"), *FString::SanitizeFloat(Damage), *FString::SanitizeFloat(Health));
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void UTPSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTPSHealthComponent, Health);
}