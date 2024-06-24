// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPERSONTEST_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UHealthComponent();

	// Gets the boolean variable that check if the player is dead
	bool IsDead();

	// Gets the current health
	float GetCurrentHealth() const;

	// Gets the health percentage
	float GetCurrentHealthPercentage() const;

	// Applies damage to the player
	void ApplyDamage(float DamageAmount);
	
	void ApplyHealing(float HealingAmount);

protected:
	
	// Called when the game starts
	virtual void BeginPlay() override;

	// Updates the health bar
	void UpdateHealthBar();

	// Variable for the max health
	float MaxHealth = 100.0f;

	// Variable for the player's current health
	float CurrentHealth;

	// Boolean variable to check if the player is dead
	bool bIsDead = false;

	// Function that is called when the player is dead
	void OnDeath();

	// Reference to the Game over widget blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameOverWidget;


public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
