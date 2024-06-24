// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.h"
#include "FirstPersonTestCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;


UCLASS(config=Game)
class AFirstPersonTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* PressShift;
public:
	
	AFirstPersonTestCharacter();

protected:
	
	virtual void BeginPlay();

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void OnPressShift();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/* Added from other member */ 

	// Reference to the health component
	UPROPERTY()
	UHealthComponent* HealthComponent;

public:
	
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

/* Added onto the template */

	// This variable checks if we want the timer to count up or down.
	UPROPERTY(EditAnywhere)
	bool bShouldTimeCountDown = true;

	// Keeps track of Minutes
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Minutes = 2;

	// Keeps track of Seconds
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Seconds = 0;

	// Reference to the EndWidget blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> EndWidget;

	// Reference to the GameOverWidget blueprint
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameOverWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HealReady;

	
	
	// Function for the timer to count up
	void CountUp();

	// Function for the timer to count down
	void CountDown();

	bool HealingReady = true;
	bool bHealTextVisible = false;

	// Function to load the EndWidget screen
	void LoadEndScreen();

	// Function to load the GameOverWidget screen
	void LoadGameOverScreen();
	void LoadHealText();

	/* Added from other files */
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	// Reference to the Player HUD
	UPROPERTY()
	UPlayerHUD* PlayerHUD;

	// Updates health
	void UpdateHealth(float HealthPercentage);

	void HandleHit(float Damage);

	// Adds damage to the player's health
	void ApplyDamage(float DamageAmount);
	
	void ApplyHealing(float HealingAmount);
	
	

	// Maximum health of player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;

	// Current health of player
	UPROPERTY(BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	// Gets the current health
	float GetCurrentHealth() const { return CurrentHealth; }

	// Updates the health bar
	void UpdateHealthBar(float HealthPercent);
	
};

