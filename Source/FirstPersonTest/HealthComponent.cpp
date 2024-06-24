// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "FirstPersonTestCharacter.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;
}

// Check if the player is dead
bool UHealthComponent::IsDead()
{
	return bIsDead;
}


// Get the current health
float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

// The the health percentage
float UHealthComponent::GetCurrentHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}


// Add damage to the player
void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (bIsDead) return;
	
	CurrentHealth -= DamageAmount;
	//UE_LOG(LogTemp,Display,TEXT("Damage Dealt"));
	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
		CurrentHealth = 0.0f;
	}
	UpdateHealthBar();
}


void UHealthComponent::ApplyHealing(float HealingAmount)
{
	if (bIsDead) return;
	CurrentHealth += HealingAmount;
	if (CurrentHealth > 100.0f)
	{
		CurrentHealth = 100.0f;
	}
	UpdateHealthBar();
}


// Updates the health bar
void UHealthComponent::UpdateHealthBar()
{
	AFirstPersonTestCharacter* PlayerCharacter = Cast<AFirstPersonTestCharacter>(GetOwner());
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->UpdateHealthBar(GetCurrentHealthPercentage());
	}
}


// Called when the player dies
void UHealthComponent::OnDeath()
{
	//UE_LOG(LogTemp, Display, TEXT("The character has died."))
	bIsDead = true;
	if (CurrentHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dead"));
		// The player has run out of health
		AFirstPersonTestCharacter* PlayerCharacter = Cast<AFirstPersonTestCharacter>(GetOwner());
		if (PlayerCharacter != nullptr)
		{
			PlayerCharacter->LoadGameOverScreen();
		}
	}
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

