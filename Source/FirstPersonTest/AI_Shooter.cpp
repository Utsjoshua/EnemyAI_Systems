// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Shooter.h"

#include "HealthComponent.h"
#include "FirstPersonTestCharacter.h"
#include "GenericPlatform/GenericPlatformCrashContext.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAI_Shooter::AAI_Shooter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAI_Shooter::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AAI_Shooter::Fire, 5.0f, true);
}

// Called to shoot to the player
void AAI_Shooter::Fire()
{
	const FVector StartLocation = GetActorLocation() + FVector(0.0f, 0.0f, 45.0f);
	const FVector ForwardVector = GetActorForwardVector();
	const FVector EndLocation = StartLocation + (ForwardVector * 5000.0f);


	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;


	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel1, CollisionParams))
	{
		
		DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Green, false, 1.0f, 0, 5.0f);
		AActor* HitActor = HitResult.GetActor();
		AFirstPersonTestCharacter* HitCharacter = Cast<AFirstPersonTestCharacter>(HitActor);
		if (HitCharacter != nullptr)
		{
			HitCharacter->ApplyDamage(20.0f);
		}
	}	
	else
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 5.0f); 

	}

	
}


// Called every frame
void AAI_Shooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Ensure we have a valid player controller
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController && PlayerController->GetPawn())
	{
		// Get the player's location
		const FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();

		// Calculate the rotation to face the player
		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation);

		// Set the AI's rotation to face the player
		SetActorRotation(LookAtRotation);
	}
	
}

// Called to bind functionality to input
void AAI_Shooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


