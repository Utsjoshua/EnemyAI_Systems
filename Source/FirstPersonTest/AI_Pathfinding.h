// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AI_Navigation.h"
#include "AI_Pathfinding.generated.h"

// Reference to the AI_Navigation class
class AAI_Navigation;

UCLASS()
class FIRSTPERSONTEST_API UAI_Pathfinding : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// Calls the populate nodes function when the world has loaded.
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// Gets a random path that could be taken by the AI from a staring location.
	TArray<FVector> GetRandomPath(const FVector& StartLocation);

	// Gets a shortest path to reach a certain target.
	TArray<FVector> GetPath(const FVector& StartLocation, const FVector& TargetLocation);

protected:

	// A list of all Navigation Nodes
	TArray<AAI_Navigation*> NavigationNodes;

private:

	// Adds all nodes in the world to the Navigation Node list
	void PopulateNodes();

	// Gets a random navigation node in the world
	AAI_Navigation* GetRandomNode();

	// Gets the closes navigation node from a target
	AAI_Navigation* GetClosestNode(const FVector& TargetLocation);

	// Gets the furthest navigation node from a target
	AAI_Navigation* GetFurthestNode(const FVector& TargetLocation);

	// Gets a path from a start navigation node to the ending navigation node
	TArray<FVector> GetPath(AAI_Navigation* StartNode, AAI_Navigation* EndNode);

	// Reconstructs a path 
	static TArray<FVector> ReconstructPath(const TMap<AAI_Navigation*, AAI_Navigation*>& CameFromMap, AAI_Navigation* EndNode);
	
};
