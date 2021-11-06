#include "Grid.h"
#include "Tile.h"
#include "GridCell.h"
#include "Engine/World.h"

UGrid::UGrid()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrid::BeginPlay()
{
	Super::BeginPlay();
}

void UGrid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bClearGrid) {
		Clear();

		bClearGrid = false;
	}
}

AGridCell* UGrid::GetGridCell(int x, int y, int z)
{
	int GridCellIndex = x * Depth * Height + y * Height + z;

	if (GridCells.IsValidIndex(GridCellIndex))
		return GridCells[GridCellIndex];
	else
		return nullptr;
}

void UGrid::Clear()
{
	// Clear and destroy all GridCells
	for (auto GridCell : GridCells) {
		GridCell->Clear();

		GridCell->Destroy();
	}

	GridCells.Empty();

	// Since references are not persistent, also destroy all attached GridCell actors
	TArray<AActor*> AttachedGridCellActors;
	
	GetOwner()->GetAttachedActors(AttachedGridCellActors);

	for (auto AttachedGridCellActor : AttachedGridCellActors) {
		AGridCell* GridCell = Cast<AGridCell>(AttachedGridCellActor);

		if (GridCell) {
			GridCell->Clear();

			// Destroy all attached Tile actors
			TArray<AActor*> AttachedTileActors;

			GridCell->GetAttachedActors(AttachedTileActors);

			for (auto AttachedTileActor : AttachedTileActors) {
				ATile* Tile = Cast<ATile>(AttachedTileActor);

				if (Tile) {
					Tile->Destroy();
				}
			}

			GridCell->Destroy();
		}
	}

	CurrentWidth = 0;
	CurrentDepth = 0;
	CurrentHeight = 0;
}

void UGrid::ForEachGridCell(TFunctionRef<void(AGridCell*)> Func)
{
	ForEachGridCell([&](AGridCell* GridCell, int x, int y, int z) {
		Func(GridCell);
	});
}

void UGrid::ForEachGridCell(TFunctionRef<void(AGridCell*, int, int, int)> Func)
{
	for (int x = 0; x < CurrentWidth; x++) {
		for (int y = 0; y < CurrentDepth; y++) {
			for (int z = 0; z < CurrentHeight; z++) {
				AGridCell* GridCell = GetGridCell(x, y, z);

				if (GridCell)
					Func(GridCell, x, y, z);
			}
		}
	}
}

void UGrid::GenerateGrid(TArray<TSubclassOf<ATile>> TileSet)
{
	if (!SizeChanged()) {
		ClearGridCells(TileSet);

		return;
	}

	Clear();

	CurrentWidth = Width;
	CurrentDepth = Depth;
	CurrentHeight = Height;
	CurrentTileSize = TileSize;
	bCurrentCentreOrigin = bCentreOrigin;

	for (int x = 0; x < CurrentWidth; x++) {
		for (int y = 0; y < CurrentDepth; y++) {
			for (int z = 0; z < CurrentHeight; z++) {
				FVector SpawnPosition = GetOwner()->GetActorLocation() + FVector(x, y, z) * CurrentTileSize;

				if (bCurrentCentreOrigin)
					SpawnPosition -= FVector(CurrentWidth - 1, CurrentDepth - 1, CurrentHeight - 1) * CurrentTileSize / 2;

				AGridCell* SpawnedGridCell = GetWorld()->SpawnActor<AGridCell>(SpawnPosition, FRotator::ZeroRotator);

				SpawnedGridCell->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);

				GridCells.Add(SpawnedGridCell);

				SpawnedGridCell->Initialise(TileSet, x, y, z);
			}
		}
	}
}

AGridCell* UGrid::GetAdjacentCell(int x, int y, int z, EDirection Direction)
{
	switch (Direction)
	{
		case EDirection::RIGHT:
			if (x > 0)
				return GetGridCell(x - 1, y, z);
			break;
		case EDirection::LEFT:
			if (x < CurrentWidth - 1)
				return GetGridCell(x + 1, y, z);
			break;
		case EDirection::BACK:
			if (y > 0)
				return GetGridCell(x, y - 1, z);
			break;
		case EDirection::FORWARD:
			if (y < CurrentDepth - 1)
				return GetGridCell(x, y + 1, z);
			break;
		case EDirection::DOWN:
			if (z > 0)
				return GetGridCell(x, y, z - 1);
			break;
		case EDirection::UP:
			if (z < CurrentHeight - 1)
				return GetGridCell(x, y, z + 1);
			break;
	}

	return nullptr;
}

AGridCell* UGrid::GetAdjacentCell(AGridCell* GridCell, EDirection Direction)
{
	if (GridCell)
		return GetAdjacentCell(GridCell->GridPosition.X, GridCell->GridPosition.Y, GridCell->GridPosition.Z, Direction);
	else
		return nullptr;
}

TArray<AGridCell*> UGrid::CreateBorders(TSubclassOf<ATile> BorderTile)
{
	TArray<AGridCell*> ChangedGridCells;

	if (!BorderTile) {
		UE_LOG(LogTemp, Warning, TEXT("Border Creation Failed: Border tile not set."));
		return ChangedGridCells;
	}

	if (CurrentHeight > 0) {
		for (int x = 0; x < CurrentWidth; x++) {
			for (int y = 0; y < CurrentDepth; y++) {
				TryCreateBorderTile(ChangedGridCells, BorderTile, x, y, 0);
				TryCreateBorderTile(ChangedGridCells, BorderTile, x, y, CurrentHeight - 1);
			}
		}
	}

	if (CurrentDepth > 0) {
		for (int x = 0; x < CurrentWidth; x++) {
			for (int z = 0; z < CurrentHeight; z++) {
				TryCreateBorderTile(ChangedGridCells, BorderTile, x, 0, z);
				TryCreateBorderTile(ChangedGridCells, BorderTile, x, CurrentDepth - 1, z);
			}
		}
	}

	if (CurrentWidth > 0) {
		for (int y = 0; y < CurrentDepth; y++) {
			for (int z = 0; z < CurrentHeight; z++) {
				TryCreateBorderTile(ChangedGridCells, BorderTile, 0, y, z);
				TryCreateBorderTile(ChangedGridCells, BorderTile, CurrentWidth - 1, y, z);
			}
		}
	}

	return ChangedGridCells;
}

void UGrid::TryCreateBorderTile(UPARAM(ref) TArray<AGridCell*>& ChangedGridCells, TSubclassOf<ATile> BorderTile, int x, int y, int z)
{
	if (!BorderTile) {
		UE_LOG(LogTemp, Warning, TEXT("Border Tile Creation Failed: Border tile not set."));
		return;
	}

	AGridCell* GridCell = GetGridCell(x, y, z);

	if (!GridCell) {
		UE_LOG(LogTemp, Warning, TEXT("Border Tile Creation Failed: Grid cell could not be found."));
		return;
	}

	GetGridCell(x, y, z)->CreateTile(BorderTile);

	ChangedGridCells.Add(GridCell);

	return;
}

void UGrid::ClearGridCells(TArray<TSubclassOf<ATile>> TileSet)
{
	for (auto GridCell : GridCells) {
		GridCell->Clear();

		GridCell->Initialise(TileSet);
	}
}

bool UGrid::SizeChanged()
{
	return	Width			!= CurrentWidth				||
			Depth			!= CurrentDepth				||
			Height			!= CurrentHeight			||
			TileSize		!= CurrentTileSize			||
			bCentreOrigin	!= bCurrentCentreOrigin;
}
