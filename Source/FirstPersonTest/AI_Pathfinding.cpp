// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_Pathfinding.h"
#include "EngineUtils.h"
#include "AI_Navigation.h"

// When the world is first loaded, add all navigation nodes to a list.
void UAI_Pathfinding::OnWorldBeginPlay(UWorld& InWorld)
{
	PopulateNodes();
}

// This is used for when the AI is Free-Roaming.
// It gets a path between the AI's start location node to a random node in the world.
TArray<FVector> UAI_Pathfinding::GetRandomPath(const FVector& StartLocation)
{
	return GetPath(GetClosestNode(StartLocation), GetRandomNode());
}

// This is used by the AI when it is either Free-Roaming or Chasing the player
// It gets a path between the AI's start location node to a target node in the world.
TArray<FVector> UAI_Pathfinding::GetPath(const FVector& StartLocation, const FVector& TargetLocation)
{
	return GetPath(GetClosestNode(StartLocation), GetClosestNode(TargetLocation));
}

// Adds all navigation nodes from the world into the Navigation nodes list variable.
void UAI_Pathfinding::PopulateNodes()
{
	NavigationNodes.Empty();

	for (TActorIterator<AAI_Navigation> It(GetWorld()); It; ++It)
	{
		NavigationNodes.Add(*It);
	}
}

// Gets a random navigation node in the world.
AAI_Navigation* UAI_Pathfinding::GetRandomNode()
{
	// If the list is empty, then do nothing.
	if (NavigationNodes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The nodes array is empty."))
		return nullptr;
	}

	// Choose a random index which will be used to access a node from the NavigationNodes list.
	const int32 RandIndex = FMath::RandRange(0, NavigationNodes.Num()-1);
	return NavigationNodes[RandIndex];
}

// Get the closest navigation node from a target location
AAI_Navigation* UAI_Pathfinding::GetClosestNode(const FVector& TargetLocation)
{
	// If the list is empty, then do nothing.
	if (NavigationNodes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The nodes array is empty."))
		return nullptr;
	}

	// Setting the closest node to null and the minimum distance to the maximum float.
	// This is to ensure that our distance between the target and node is smaller than this number.
	AAI_Navigation* ClosestNode = nullptr;
	float MinDistance = UE_MAX_FLT;

	// For each node in the list, check if the distance between the target location and the node location is shorter than the previous nodes in the list.
	for (AAI_Navigation* Node : NavigationNodes)
	{
		const float Distance = FVector::Distance(TargetLocation, Node->GetActorLocation());

		// If the new distance is smaller than the minimum distance, then set the new value of MinDistance and the closest node.
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestNode = Node;
		}
	}

	return ClosestNode;
}

// Get the furthest navigation node from a target location
AAI_Navigation* UAI_Pathfinding::GetFurthestNode(const FVector& TargetLocation)
{
	// If the list is empty, then do nothing.
	if (NavigationNodes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The nodes array is empty."))
		return nullptr;
	}

	// Setting the furthest node to null and the maximum distance to the minimum float.
	// This is to ensure that our distance between the target and node is larger than this number.
	AAI_Navigation* FurthestNode = nullptr;
	float MaxDistance = -1.0f;

	// For each node in the list, check if the distance between the target location and the node location is longer than the previous nodes in the list.
	for (AAI_Navigation* Node : NavigationNodes)
	{
		const float Distance = FVector::Distance(TargetLocation, Node->GetActorLocation());
		
		// If the new distance is larger than the minimum distance, then set the new value of MaxDistance and the furthest node.
		if (Distance > MaxDistance)
		{
			MaxDistance = Distance;
			FurthestNode = Node;
		}
	}

	return FurthestNode;
}

// Gets a path between the start and end navigation node
TArray<FVector> UAI_Pathfinding::GetPath(AAI_Navigation* StartNode, AAI_Navigation* EndNode)
{

	// Check if either nodes are empty or null.
	if (!StartNode || !EndNode)
	{
		UE_LOG(LogTemp, Error, TEXT("Either the start or end node are nullptrs."))
		return TArray<FVector>();
	}

	// Setup the open set and add the start node.
	TArray<AAI_Navigation*> OpenSet;
	OpenSet.Add(StartNode);

	// StartNode->GScore = UE_MAX_FLT;
	// StartNode->HScore = FVector::Distance(StartNode->GetActorLocation(), EndNode->GetActorLocation());

	// Setup the maps that will hold the GScores, HScores and CameFrom
	TMap<AAI_Navigation*, float> GScores, HScores;
	TMap<AAI_Navigation*, AAI_Navigation*> CameFrom;

	// Setup the start nodes G and H score.
	GScores.Add(StartNode, 0);
	HScores.Add(StartNode, FVector::Distance(StartNode->GetActorLocation(), EndNode->GetActorLocation()));
	CameFrom.Add(StartNode, nullptr);

	// loop while the OpenSet list is not empty
	while (!OpenSet.IsEmpty())
	{
		// Find the node in the open set with the lowest FScore.
		AAI_Navigation* CurrentNode = OpenSet[0];
		for (int32 i = 1; i < OpenSet.Num(); i++)
		{
			// We can be sure that all the nodes in the open set have already had their GScores and HScores calculated.
			if (GScores[OpenSet[i]] + HScores[OpenSet[i]] < GScores[CurrentNode] + HScores[CurrentNode])
			{
				CurrentNode = OpenSet[i];
			}
		}

		// Remove the current node from the OpenSet list
		OpenSet.Remove(CurrentNode);

		if (CurrentNode == EndNode)
		{
			// Reconstruct the path and get the positions of each of the nodes in the path.
			UE_LOG(LogTemp, Display, TEXT("A path has been found"))
			return ReconstructPath(CameFrom, EndNode);
		}

		// For each node in the adjacent node list:
		for (AAI_Navigation* AdjacentNode : CurrentNode->AdjacentNodes)
		{
			// Check if the connected navigation node is not null
			if (!AdjacentNode)
			{
				continue;
			}
			
			const float TentativeGScore = GScores[CurrentNode] + FVector::Distance(CurrentNode->GetActorLocation(), AdjacentNode->GetActorLocation());

			// Check if the adjacent node doesn't have a GScore, then set the values of the GScores, HScores and CameFrom variables
			if (!GScores.Contains(AdjacentNode))
			{
				GScores.Add(AdjacentNode, UE_MAX_FLT);
				HScores.Add(AdjacentNode, FVector::Distance(AdjacentNode->GetActorLocation(), EndNode->GetActorLocation()));
				CameFrom.Add(AdjacentNode, nullptr);
			}

			// If the TentativeGScore is less than the current g score, then update this nodes scores and came from.
			if (TentativeGScore < GScores[AdjacentNode])
			{
				CameFrom[AdjacentNode] = CurrentNode;
				GScores[AdjacentNode] = TentativeGScore;
				
				// If the adjacent node isn't already in the Open set list, then add connected node to the open set.
				if (!OpenSet.Contains(AdjacentNode))
				{
					OpenSet.Add(AdjacentNode);
				}
			}
		}
	}

	// If the OpenSet list is empty, then just return an empty array.
	return TArray<FVector>();
	
}

// Reconstructs a path using a node from where the path comes from and to which node the path should end.
TArray<FVector> UAI_Pathfinding::ReconstructPath(const TMap<AAI_Navigation*, AAI_Navigation*>& CameFromMap, AAI_Navigation* EndNode)
{
	TArray<FVector> NodeLocations;

	const AAI_Navigation* NextNode = EndNode;

	// While the next node is still the ending node, add its location to a list.
	while(NextNode)
	{
		NodeLocations.Push(NextNode->GetActorLocation());
		NextNode = CameFromMap[NextNode];
	}

	return NodeLocations;
}

