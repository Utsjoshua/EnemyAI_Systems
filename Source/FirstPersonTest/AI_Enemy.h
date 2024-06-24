// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI_Pathfinding.h"
#include "FirstPersonTestCharacter.h"
#include "AI_Enemy.generated.h"

class UPawnSensingComponent;
class AFirstPersonTestCharacter;
class UAI_Pathfinding;

// This keeps track on whether the AI is Free-roaming or Chasing the player down
UENUM(BlueprintType)
enum class EAI_State : uint8
{
	FreeRoam,
	Chasing
};

UCLASS()
class FIRSTPERSONTEST_API AAI_Enemy : public ACharacter
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	AAI_Enemy();

	// Called every frame. About 60fps.
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// This variable is how active the AI should be. 1 - Not active, 20 - always active.
    UPROPERTY(EditAnywhere)
    int AILevel;

	// This boolean variable can allow for the AI to become more active as time goes on.
	UPROPERTY(EditAnywhere)
	bool bShouldAILevelGrowByTime;

	// This boolean variable can allow for the AI level to affect the AI's movement speed and wait times.
	UPROPERTY(EditAnywhere)
	bool bShouldAILevelAffectSpeedAndTime;

	// This boolean variable can allow for the AI to become more active whenever it has chased a player.
	UPROPERTY(EditAnywhere)
	bool bShouldAILevelGrowByChase;

	// This boolean variable can allow the AI to reach further to attack the player based on the AI Level.
	UPROPERTY(EditAnywhere)
	bool bShouldAILeveAffectAttackRange;

	// This keeps track of time and is used for when the AI level is growing more active.
	UPROPERTY(VisibleAnywhere)
	float TimeCount = 0.0f;

	// This is the amount of time that should pass for the AI to level up. 
	UPROPERTY(EditAnywhere)
	float LevelUpEveryTimeCount = 10.0f;
	
	// Movement speed of the AI
	float MovementSpeed;

	// Check if the AI is waiting 
	bool bIsWaiting = false;
	
	// Check if the AI is calling the FreeRoam function
	bool bIsFreeRoamCalled = false;

	// Check if the AI has made it to its destination
	bool bHasMadeItToDestination = false;

	// Check if the AI is active
	bool bIsActive;

	// Check if the AI has attacted lately
	bool bHasAttacked = false;

	// Calls a timer to handle with when our AI is waiting
	FTimerHandle WaitTimerHandle;

	// Calls a timer for the attack cooldown
	FTimerHandle AttackCooldownTimerHandle;

	// Based on the AI level, this function effects how active the AI will move.
	void AIActivity();

	// The AI stops moving and waits a random amount of time.
	void StartWaiting();

	// Called after the Start Waiting function to mark the end of a wait
	void Continue();

	// Move the AI to a new location
	void MoveAI();

	// The AI moves to random locations at random times
    void FreeRoam();
	
	// The AI finds the shortest possible path to reach the player
	void Chase();
	
	void EndAttackCooldown();

	// Checking if the AI can still see the player
	void UpdateSight();

	// Checking if the Player is heard by the AI
	UFUNCTION()
	void OnSensedPawn(APawn* SensedActor);

	// Calls on the Pathfinding subsystem class
	UPROPERTY()
	UAI_Pathfinding* PathfindingSubsystem;

	// Calls on the Pawn Sensing Component on the AI
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	// Calls on the Sensed Character that the AI Senses
	UPROPERTY()
	AFirstPersonTestCharacter* SensedCharacter = nullptr;

	// A list of key node locations which the AI is planning to go to.
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> CurrentPath;

	// The current state of the AI whether it is free-roaming or chasing the player
	UPROPERTY(EditAnywhere)
	EAI_State CurrentState = EAI_State::FreeRoam;

	// The distance where it is considered that a destination is reached.
	UPROPERTY(EditAnywhere)
	float PathfindingError = 150.0f; // 150 cm from target by default.

	// The player's location
	FVector PlayerLocation;

	// The location of the AI
	FVector AILocation = GetActorLocation();

	// This is the distance between the player and AI
	float DistanceToPlayer;

	// This is the distance range where the player is considered very close and the AI is able to damage the player.
	float AttackRange = 300.0f;
};