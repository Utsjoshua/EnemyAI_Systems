// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AI_Navigation.generated.h"

UCLASS()
class FIRSTPERSONTEST_API AAI_Navigation : public AActor
{
	GENERATED_BODY()

	// Works together with AI_Pathfinding class
	friend class UAI_Pathfinding;
	
public:	
	// Sets default values for this actor's properties
	AAI_Navigation();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The position of this node itself
	UPROPERTY(VisibleAnywhere)
	USceneComponent* LocationComponent;

	// A list of other nodes that are connected to this node itself 
	UPROPERTY(EditAnywhere)
	TArray<AAI_Navigation*> AdjacentNodes;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldTickIfViewportsOnly() const override;

};
