// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_Enemy.h"
#include "FirstPersonTestCharacter.h"
#include "AI_Pathfinding.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AAI_Enemy::AAI_Enemy()
{
 	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Creates an object for the AI which is to help sense whether a player is nearby
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");
}

// Called when the game starts or when spawned
void AAI_Enemy::BeginPlay()
{
	Super::BeginPlay();

	PathfindingSubsystem = GetWorld()->GetSubsystem<UAI_Pathfinding>();
	if (PathfindingSubsystem)
	{
		CurrentPath = PathfindingSubsystem->GetRandomPath(GetActorLocation());
	}
	
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find the PathfindingSubsystem"))
	}
	
	if (PawnSensingComponent)
	{
		// Set up Pawn sensing variables:
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AAI_Enemy::OnSensedPawn);
		PawnSensingComponent->SetPeripheralVisionAngle(50.0f);
		PawnSensingComponent->SightRadius = 1400.0f;
	}
	
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Something went wrong with the pawn sensing component"))
	}
}

// Called every frame
void AAI_Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Display, TEXT("Ticked"))
	
	TimeCount += DeltaTime;
	
	// Every certain second/s (LevelUpEveryTimeCount), the AI_Level will increase by 1.
	if (TimeCount > LevelUpEveryTimeCount)
	{
	    if (bShouldAILevelGrowByTime)
        {
        	UE_LOG(LogTemp, Display, TEXT("AI Level has increased"))
        	AILevel += 1;
        }
        TimeCount = 0.0f;
	}
	
	UpdateSight();

	// Check the AI state:
	switch(CurrentState)
	{
		// If the AI is in free-roam mode:
		case EAI_State::FreeRoam:

			MovementSpeed = 0.25f;
		
			// 1. If the AI has made it to its destination and finished waiting, then check if the AI will be Active or not.
			if ((!bIsFreeRoamCalled && bHasMadeItToDestination && !bIsWaiting) || (!bIsActive && !bIsWaiting))
			{
				AIActivity();
			}

			// 2. If the AI will be active, then move it to the next random node location.
			if (bIsActive && !bIsWaiting)
			{
				FreeRoam();
			}

			// 3. If the AI has made it to its destination node, then wait a random amount of time.
			if (bHasMadeItToDestination && !bIsWaiting)
			{
				bIsFreeRoamCalled = false;
				bIsWaiting = true;
				StartWaiting();
				bIsActive = false;
				bHasMadeItToDestination = false;
			}
		
			// 4. If the AI sees a player, the AI will now start chasing
			if (SensedCharacter && AILevel != 0)
			{
				CurrentState = EAI_State::Chasing;
				CurrentPath.Empty();
			}
			break;

		// If the AI is in chasing mode:
		case EAI_State::Chasing:

			MovementSpeed = 1.0f;

			// Chase player
			Chase();

			// Check if the sensed player is very close to the AI
			if (SensedCharacter)
			{
				PlayerLocation = SensedCharacter->GetActorLocation();
				AILocation = GetActorLocation();
				DistanceToPlayer = FVector::Distance(PlayerLocation, AILocation);

				// Checking if the player is in range to be attacked by the AI
				if (bShouldAILeveAffectAttackRange)
				{
					if (DistanceToPlayer <= AttackRange + (AILevel * 5))
					{
						//The NPC is within the circular range of the player.
						UE_LOG(LogTemp, Display, TEXT("The AI has reached the player"))
						
						if (!bHasAttacked)
						{
						    // The AI attacks the player
							UE_LOG(LogTemp, Display, TEXT("The AI has attacked the player"))
                        	SensedCharacter->ApplyDamage(10.0f);
                        	bHasAttacked = true;
                        	GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AAI_Enemy::EndAttackCooldown, 1.0f, false);
						}
					}
				}
				else
				{
					if (DistanceToPlayer <= AttackRange)
					{
						//The NPC is within the circular range of the player.
						//UE_LOG(LogTemp, Display, TEXT("The AI has reached the player"))
						
						if (!bHasAttacked)
                        {
                            // The AI attacks the player
                        	UE_LOG(LogTemp, Display, TEXT("The AI has attacked the player"))
                            SensedCharacter->ApplyDamage(10.0f);
                            bHasAttacked = true;
                            GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AAI_Enemy::EndAttackCooldown, 1.0f, false);
                        }
					}
				}
			}

			// If the AI loses a player, the AI will now start free-roaming
			if (!SensedCharacter)
			{
				CurrentState = EAI_State::FreeRoam;
			}
			break;

		// Default case just in case when something weird happens where the state is neither Chasing nor free Roaming:
		// Just set the AI to be free Roaming
		default:
			CurrentState = EAI_State::FreeRoam;
	}
}

// Called to bind functionality to input
void AAI_Enemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// The AI free-roams around to random locations at random times
void AAI_Enemy::FreeRoam()
{
	UE_LOG(LogTemp, Display, TEXT("Free roaming"))
	
	bIsFreeRoamCalled = true;

	// Get a random path of nodes
	if (CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetRandomPath(GetActorLocation());
	}

	// Move the AI
	MoveAI();
}

// Check to see if the AI is active or not.
void AAI_Enemy::AIActivity()
{
	// If the AI Level is 0, then the AI will not move
	if (AILevel == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("AI Level is set to 0, so it won't move"))
		return;
	}

	// Generate a random number and only move the AI if this random number is less than or equal to the AI Level.
	const int RandomNumber = FMath::FRandRange(1.0f,20.0f);
	if (RandomNumber <= AILevel)
	{
		UE_LOG(LogTemp, Display, TEXT("AI is active"))
		bIsActive = true;
	}
	
	// If the random number is bigger than the AI level, then the AI will wait for its next turn.
	else
	{
		UE_LOG(LogTemp, Display, TEXT("AI is NOT active"))
		StartWaiting();
	}
}

// Move the AI to a new location
void AAI_Enemy::MoveAI()
{
	if (CurrentPath.IsEmpty())
	{
		return;
	}

	// Get the direction between 2 points
	FVector Direction = CurrentPath[CurrentPath.Num()-1] - GetActorLocation();
	Direction.Normalize();

	UE_LOG(LogTemp, Display, TEXT("Moving"))

	if (bShouldAILevelAffectSpeedAndTime)
    {
    	AddMovementInput(Direction, MovementSpeed + (AILevel / 100));
    }
	else
	{
		// Move the AI to the next location with this direction
		AddMovementInput(Direction, MovementSpeed);
	}

	// Check if it is close to the current stage of the path
	if (FVector::Distance(GetActorLocation(), CurrentPath[CurrentPath.Num() - 1]) < PathfindingError)
	{
		bHasMadeItToDestination = true;
		UE_LOG(LogTemp, Display, TEXT("Made it to destination"))
		CurrentPath.Pop();
	}
}

// The AI waits a random amount of time.
void AAI_Enemy::StartWaiting()
{
	UE_LOG(LogTemp, Display, TEXT("Started Waiting"))

	float MaxWaitTime = 5.0f;

	if (bShouldAILevelAffectSpeedAndTime)
	{
		MaxWaitTime -= 0.1f * AILevel;
		if (MaxWaitTime <= 0.0f)
		{
			MaxWaitTime = 0.1f;
		}
	}
	
	// Generate a random wait time between MinWaitTime and MaxWaitTime (in seconds)
	const float WaitTime = FMath::RandRange(0.0f, MaxWaitTime);

	// Set a flag to indicate that the NPC is waiting
	bIsWaiting = true;

	// Start a timer to resume movement after WaitTime seconds
	GetWorldTimerManager().SetTimer(WaitTimerHandle, this, &AAI_Enemy::Continue, WaitTime, false);
}

// When the AI is finished waiting, call this function to end the waiting time and continue the next round of functions.
void AAI_Enemy::Continue()
{
	UE_LOG(LogTemp, Display, TEXT("Finished Waiting"))
	bIsWaiting = false;
}

// The AI chases the player by finding the shortest path to it.
void AAI_Enemy::Chase()
{
	UE_LOG(LogTemp, Display, TEXT("Chasing Player"))
	
	if (!SensedCharacter)
	{
		return;
	}
	
	if (CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), SensedCharacter->GetActorLocation());
	}
	MoveAI();
}

// This marks the end of an attack cooldown.
void AAI_Enemy::EndAttackCooldown()
{
	bHasAttacked = false;
	UE_LOG(LogTemp, Display, TEXT("The AI is ready to attack again"))
}

// The AI senses a nearby player
void AAI_Enemy::OnSensedPawn(APawn* SensedActor)
{
	if (AFirstPersonTestCharacter* Player = Cast<AFirstPersonTestCharacter>(SensedActor))
	{
		SensedCharacter = Player;
		UE_LOG(LogTemp, Display, TEXT("Saw Player"))
	}
}

// The AI checks if it has lost track of the player after seeing them before
void AAI_Enemy::UpdateSight()
{
	if (!SensedCharacter) return;
	
	if (PawnSensingComponent)
	{
		// The AI has lost track of the player
		if (!PawnSensingComponent->HasLineOfSightTo(SensedCharacter))
		{
			SensedCharacter = nullptr;
			UE_LOG(LogTemp, Display, TEXT("Lost Player"))
			
			if (bShouldAILevelGrowByChase)
			{
				UE_LOG(LogTemp, Display, TEXT("AI Level has increased"))
				AILevel += 1;
			}
		}
	}
}