// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Navigation.h"

// Sets default values
AAI_Navigation::AAI_Navigation()
{
	
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	
	// Creating a Location Component onto the node. 
	LocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location Component"));
	SetRootComponent(LocationComponent);

}

// Called when the game starts or when spawned
void AAI_Navigation::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAI_Navigation::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);

	// Below is not needed for a real game environment. It is just used for testing and debugging.

	// This is for testing and debugging.
	// It sets the default colour of the node to blue.
	// If this node appears in the "AdjacentNodes" list then color it red since it is not right.
	FColor SphereColor = FColor::Blue;
	if (AdjacentNodes.Contains(this))
	{
		SphereColor = FColor::Red;
	}

	// Testing and debug sphere around the nodes.
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 4, SphereColor, false, -1, 0, 5.0f);

	// Create debug lines between each adjacent node.
	// Red lines mean that you could travel only one way from a node to another.
	// Green lines mean you can travel in both directions from a node to another.
	for (const AAI_Navigation* Node : AdjacentNodes)
	{
		if (Node)
		{
			FColor LineColor = FColor::Red;
			if (Node->AdjacentNodes.Contains(this))
			{
				LineColor = FColor::Green;
			}
			//DrawDebugLine(GetWorld(), GetActorLocation(), Node->GetActorLocation(),
				//LineColor, false, -1, 0, 5.0f);
		}
	}

}

bool AAI_Navigation::ShouldTickIfViewportsOnly() const
{
	return true;
}

