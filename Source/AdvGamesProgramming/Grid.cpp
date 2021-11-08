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

UGridCell* UGrid::GetGridCell(int x, int y, int z)
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
		if (GridCell)
			GridCell->Clear();

		//GridCell->Destroy();
	}

	GridCells.Empty();

	// Since references are not persistent, also destroy all attached Tile actors
	TArray<AActor*> AttachedTileActors;
	
	GetOwner()->GetAttachedActors(AttachedTileActors);

	for (auto AttachedTileActor : AttachedTileActors) {
		ATile* Tile = Cast<ATile>(AttachedTileActor);

		if (Tile) {
			Tile->Destroy();
		}
	}

	CurrentWidth = 0;
	CurrentDepth = 0;
	CurrentHeight = 0;
}

void UGrid::ForEachGridCell(TFunctionRef<void(UGridCell*)> Func)
{
	ForEachGridCell([&](UGridCell* GridCell, int x, int y, int z) {
		Func(GridCell);
	});
}

void UGrid::ForEachGridCell(TFunctionRef<void(UGridCell*, int, int, int)> Func)
{
	for (int x = 0; x < CurrentWidth; x++) {
		for (int y = 0; y < CurrentDepth; y++) {
			for (int z = 0; z < CurrentHeight; z++) {
				UGridCell* GridCell = GetGridCell(x, y, z);

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
				FVector SpawnPosition = GetOwner()->GetActorLocation() + FVector(x, y, z) * CurrentTileSize * GetOwner()->GetActorScale3D();

				if (bCurrentCentreOrigin)
					SpawnPosition -= FVector(CurrentWidth - 1, CurrentDepth - 1, CurrentHeight - 1) * CurrentTileSize / 2 * GetOwner()->GetActorScale3D();

				UGridCell* SpawnedGridCell = NewObject<UGridCell>();

				//SpawnedGridCell->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false));

				GridCells.Add(SpawnedGridCell);

				SpawnedGridCell->Initialise(GetOwner(), TileSet, SpawnPosition, x, y, z);
			}
		}
	}
}

UGridCell* UGrid::GetAdjacentCell(int x, int y, int z, EDirection Direction)
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

UGridCell* UGrid::GetAdjacentCell(UGridCell* GridCell, EDirection Direction)
{
	if (GridCell)
		return GetAdjacentCell(GridCell->GridPosition.X, GridCell->GridPosition.Y, GridCell->GridPosition.Z, Direction);
	else
		return nullptr;
}

TArray<UGridCell*> UGrid::CreateBorders(TSubclassOf<ATile> BorderTile)
{
	TArray<UGridCell*> ChangedGridCells;

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

void UGrid::TryCreateBorderTile(UPARAM(ref) TArray<UGridCell*>& ChangedGridCells, TSubclassOf<ATile> BorderTile, int x, int y, int z)
{
	if (!BorderTile) {
		UE_LOG(LogTemp, Warning, TEXT("Border Tile Creation Failed: Border tile not set."));
		return;
	}

	UGridCell* GridCell = GetGridCell(x, y, z);

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
		if (!GridCell)
			continue;

		GridCell->Clear();

		GridCell->Initialise(GetOwner(), TileSet);
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
