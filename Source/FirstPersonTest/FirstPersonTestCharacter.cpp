// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPersonTestCharacter.h"
#include "FirstPersonTestProjectile.h"
#include "Animation/AnimInstance.h"
//#include "AITestsCommon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// AFirstPersonTestCharacter
UUserWidget* HealText;

AFirstPersonTestCharacter::AFirstPersonTestCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	CurrentHealth = MaxHealth;
	
}

void AFirstPersonTestCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	/* Added from the template */

	// Create the setup for the Countdown Timer
	FTimerHandle TimerHandle;

	// Check if we want the time to count down or up.
	if (bShouldTimeCountDown)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AFirstPersonTestCharacter::CountDown, 1.0f, true, 0.0f);
	}
	else
	{
		Minutes = 0;
		Seconds = 0;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AFirstPersonTestCharacter::CountUp, 1.0f, true, 0.0f);
	}

	/* Added from other member */
	
	// Loading the health bar onto the screen
	if (IsLocallyControlled() && PlayerHUDClass)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerHUD = CreateWidget<UPlayerHUD>(PlayerController,PlayerHUDClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}
	}
	
	UpdateHealthBar(1.0f);
	
}

//////////////////////////////////////////////////////////////////////////// Input

void AFirstPersonTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPersonTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPersonTestCharacter::Look);
		
		EnhancedInputComponent->BindAction(PressShift, ETriggerEvent::Triggered, this, &AFirstPersonTestCharacter::OnPressShift);
	}
}


void AFirstPersonTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFirstPersonTestCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFirstPersonTestCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AFirstPersonTestCharacter::GetHasRifle()
{
	return bHasRifle;
}

/* Added from the template */

// This function is to get the timer to count up.
void AFirstPersonTestCharacter::CountUp()
{
	if(Seconds == 59)
	{
		Minutes += 1;
		Seconds = 0;
	}
	else
	{
		Seconds += 1;
	}
}

// This function is to get the timer to count down.
void AFirstPersonTestCharacter::CountDown()
{
	if (Seconds != 0)
	{
		Seconds -= 1;
	}
	else
	{
		if (Minutes == 0)
		{
			// The time has run out.
			LoadEndScreen();
		}
		else
		{
			Minutes -= 1;
			Seconds = 59;
		}
	}
	if (Minutes <= 1 && Seconds <= 59 && HealingReady == true)
	{
		LoadHealText();
	}
	
	
}

// This loads the End screen after the timer has finished counting down.
void AFirstPersonTestCharacter::LoadEndScreen()
{
	UUserWidget* TimeUpWidget = CreateWidget<UUserWidget>(GetWorld(), EndWidget);
	if (TimeUpWidget)
	{
		TimeUpWidget->AddToViewport();

		// Setting the input mode to UI so we can stop controlling the player character
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

// This loads the Game Over screen
void AFirstPersonTestCharacter::LoadGameOverScreen()
{
	UUserWidget* GameOverScreen = CreateWidget<UUserWidget>(GetWorld(), GameOverWidget);
	if (GameOverScreen)
	{
		GameOverScreen->AddToViewport();

		// Setting the input mode to UI so we can stop controlling the player character
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->SetShowMouseCursor(true);
		}
	}
}


void AFirstPersonTestCharacter::LoadHealText()
{
		if (Minutes <= 1 && Seconds <= 59 && HealingReady == true && !bHealTextVisible && HealthComponent->GetCurrentHealth() < 100.0f)
		{
			HealText = CreateWidget<UUserWidget>(GetWorld(), HealReady);
			HealText->SetVisibility(ESlateVisibility::Visible);
			HealText->AddToViewport();
			bHealTextVisible = true;
			UE_LOG(LogTemp, Warning, TEXT("Text added"));
		}
		if (!HealingReady)
		{
			HealText->SetVisibility(ESlateVisibility::Hidden);
			HealText->RemoveFromParent();
			UE_LOG(LogTemp, Warning, TEXT("Text removed"));
		}
	} 

/* Added from other member */

// Updates the health bar 
void AFirstPersonTestCharacter::UpdateHealthBar(float HealthPercent)
{
	/*if (HealthComponent)
	{
		CurrentHealth = HealthComponent->GetCurrentHealthPercentage();

	}*/
	if (IsLocallyControlled() && PlayerHUD)
		{
				PlayerHUD->SetHealthBar(HealthPercent);
		}
}



void AFirstPersonTestCharacter::HandleHit(float Damage)
{
	ApplyDamage(Damage); //calling apply damage to handle hit damage
}


// Called whenever the player takes damage
void AFirstPersonTestCharacter::ApplyDamage(float DamageAmount)
{
	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(DamageAmount);
	}
}
void AFirstPersonTestCharacter::ApplyHealing(float HealingAmount)
{
	if (HealthComponent)
	{
		HealthComponent->ApplyHealing(HealingAmount);
	}
}

void AFirstPersonTestCharacter::OnPressShift()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController->IsInputKeyDown(EKeys::LeftShift) && Minutes <= 1 && Seconds <= 59 && HealingReady == true && HealthComponent->GetCurrentHealth() < 100.0f)
	{
		ApplyHealing(50.0f);
		HealingReady = false;
		LoadHealText();
		UE_LOG(LogTemp, Warning, TEXT("Healing complete"));
	}
}