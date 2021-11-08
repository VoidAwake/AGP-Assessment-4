#include "WFCStructure.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Direction.h"
#include "DirectionUtility.h"
#include "Net/UnrealNetwork.h"

AWFCStructure::AWFCStructure()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));

	Grid = CreateDefaultSubobject<UGrid>(TEXT("Grid Component"));

	AddOwnedComponent(Grid);

	Directions = {
		EDirection::LEFT,
		EDirection::RIGHT,
		EDirection::FORWARD,
		EDirection::BACK,
		EDirection::UP,
		EDirection::DOWN
	};

	bReplicates = true;
}

void AWFCStructure::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority) {
		ServerGenerate();
	}
}

void AWFCStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerate) {
		Generate();

		bRegenerate = false;
	}

	if (bGenerateGrid) {
		if (Grid)
			Grid->GenerateGrid(TileSet->TileSet);

		bGenerateGrid = false;
	}

	if (bCreateBorders) {
		CreateBorders();

		bCreateBorders = false;
	}

	if (bTestObserve) {
		Observe();

		bTestObserve = false;
	}

	if (bTestPropagate) {
		Propagate();

		bTestPropagate = false;
	}

	if (bTestBacktrack) {
		//Backtrack();

		bTestBacktrack = false;
	}
}

bool AWFCStructure::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AWFCStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWFCStructure, Seed);
}

void AWFCStructure::ServerGenerate_Implementation()
{
	Seed = FMath::Rand();

	UE_LOG(LogTemp, Warning, TEXT("Server generating with seed: %i"), Seed);

	Generate();
}

void AWFCStructure::Generate()
{
	FMath::RandInit(Seed);

	if (!Grid)
		return;

	if (!TileSet)
		return;

	//for (int i = 0; i < MaxGenerationAttempts; i++) {
		Grid->GenerateGrid(TileSet->TileSet);

		if (bCleanBorders) {
			CreateBorders();
		}

		bool bContinue = true;

		bool bContradiction = false;

		while (Observe() == 0 && bContinue) {
			bContradiction = !Propagate();

			//if (bContradiction)
			//	bContradiction = !Backtrack();

			//if (bContradiction)
			//	bContinue = false;

			if (bStopOnContradiction && bContradiction)
				bContinue = false;
		}

		if (!bContradiction) {
			UE_LOG(LogTemp, Warning, TEXT("Success"));

			//break;
		}
	//}
}

int AWFCStructure::Observe()
{
	UGridCell* CellWithMinEntropy = GetCellWithMinEntropy();

	if (!CellWithMinEntropy)
		return 1; // Complete or failed

	// Store the observation in the backtrack array

	UObservation* Observation = NewObject<UObservation>(this);

	Observation->Initialise(CellWithMinEntropy);

	BacktrackQueue.Push(Observation);

	CellWithMinEntropy->Observe();

	ChangedCellsQueue.Enqueue(CellWithMinEntropy);

	//UE_LOG(LogTemp, Warning, TEXT("Old Wave Length: %i"), BacktrackQueue[BacktrackQueue.Num() - 1]->OldWave.Num());

	return 0; // Continue
}

UGridCell* AWFCStructure::GetCellWithMinEntropy()
{
	if (!TileSet)
		return nullptr;

	int MinEntropy = TileSet->TileSet.Num() + 1;
	UGridCell* CellWithMinEntropy = nullptr;

	Grid->ForEachGridCell([&](UGridCell* GridCell) {
		int WaveSize = GridCell->Wave.Num();

		if (WaveSize == MinEntropy) {
			// TODO: Not an even random distribution
			if (FMath::RandBool()) {
				MinEntropy = WaveSize;
				CellWithMinEntropy = GridCell;
			}
		}
		else if (WaveSize < MinEntropy && WaveSize > 1) {
			MinEntropy = GridCell->Wave.Num();
			CellWithMinEntropy = GridCell;
		}
	});

	return CellWithMinEntropy;
}

bool AWFCStructure::Propagate(bool bEnableBacktrack)
{
	if (!Grid)
		return false;

	while (!ChangedCellsQueue.IsEmpty()) {
		UGridCell* GridCell = *(ChangedCellsQueue.Peek());
		ChangedCellsQueue.Pop();

		for (auto Direction : Directions) {
			UGridCell* AdjacentCell = Grid->GetAdjacentCell(GridCell, Direction);

			if (!AdjacentCell)
				continue;

			bool bContradiction = RemoveDisallowedTileTypes(GridCell, AdjacentCell, Direction);

			if (bContradiction) {
				UE_LOG(LogTemp, Warning, TEXT("Contradiction"));
				return false;
			}
		}			
	}

	return true;
}

bool AWFCStructure::RemoveDisallowedTileTypes(UGridCell* GridCell, UGridCell* AdjacentGridCell, EDirection Direction)
{
	TArray<TSubclassOf<ATile>> TilesToRemove;

	for (auto AdjacentWaveTile : AdjacentGridCell->Wave) {
		// Check if the the tile type in the adjacent wave is allowed
		if (!GridCell->Allows(AdjacentWaveTile, Direction)) {
			TilesToRemove.Add(AdjacentWaveTile);

			//FString TileName = AdjacentWaveTile->GetDefaultObject<ATile>()->GetName();

			//UE_LOG(LogTemp, Warning, TEXT("    %s disallowed"), *TileName);
		}
	}

	if (TilesToRemove.Num() > 0) {
		//if (bEnableBacktrack) {
		//	UPropagation* Propagation = NewObject<UPropagation>(this);

		//	BacktrackQueue[BacktrackQueue.Num() - 1]->Propagations.Push(Propagation);

		//	Propagation->Initialise(AdjacentGridCell);
		//}

		ChangedCellsQueue.Enqueue(AdjacentGridCell);
	}

	return !AdjacentGridCell->RemoveTileTypesFromWave(TilesToRemove);
}

void AWFCStructure::CreateBorders()
{
	if (!Grid) {
		UE_LOG(LogTemp, Error, TEXT("Border creation failed: No grid found"));
		return;
	}

	auto ChangedGridCells = Grid->CreateBorders(BorderTile);

	for (auto ChangedGridCell : ChangedGridCells) {
		ChangedCellsQueue.Enqueue(ChangedGridCell);
	}

	Propagate(false);
}

//bool AWFCStructure::Backtrack()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Attempting Backtrack"));
//
//	if (BacktrackQueue.Num() == 0) {
//		UE_LOG(LogTemp, Warning, TEXT("Backtrack failed: Backtrack queue was empty"));
//		return false;
//	}
//
//	ChangedCellsQueue.Empty();
//
//	UObservation* LastObservation = BacktrackQueue.Pop();
//
//	// TEST: Undo the propagations, not removing tiles.
//
//	while (LastObservation->Propagations.Num() > 0) {
//		UPropagation* Propagation = LastObservation->Propagations.Pop();
//
//		Propagation->GridCell->RestoreWave();
//	}
//
//	// Undo the observation
//	return LastObservation->GridCell->RestoreWave();
//}
