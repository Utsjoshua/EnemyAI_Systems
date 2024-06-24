// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FirstPersonTestCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "HealthComponent.h"
#include "AI_Shooter.generated.h"

UCLASS()
class FIRSTPERSONTEST_API AAI_Shooter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFirstPersonTestCharacter> CharacterClass;
	
	UPROPERTY()
	AFirstPersonTestCharacter* Character;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHealthComponent> HealthComponentClass;
	
	UPROPERTY()
	UHealthComponent* HealthComponent;

public:
	// Sets default values for this character's properties
	AAI_Shooter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void Fire();
	
	FTimerHandle FireTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BulletStartPosition;
};
