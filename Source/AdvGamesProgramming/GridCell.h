#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tile.h"
#include "Direction.h"
#include "Propagation.h"
#include "GridCell.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API UGridCell : public UObject
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere)
	TArray<TSubclassOf<ATile>> Wave;

	void Initialise(AActor* ParentArg, TArray<TSubclassOf<ATile>> TileSet, FVector WorldPositionArg, TSubclassOf<ATile> EmptyTileArg, int x, int y, int z);

	void Initialise(AActor* ParentArg, TArray<TSubclassOf<ATile>> TileSet);

	bool Allows(TSubclassOf<ATile> NeighbourTileType, EDirection Direction);

	void Observe();

	//bool RestoreWave();

	void CreateTile(TSubclassOf<ATile> TileTypeToSpawn);

	void Clear();

	bool RemoveTileTypesFromWave(TArray<TSubclassOf<ATile>> TileTypesToRemove);

	//void RecordWave();

	FVector GridPosition;

	ATile* Tile;

	AActor* Parent;

	FVector WorldPosition;

	TSubclassOf<ATile> EmptyTile;

private:
	//TArray<UPropagation*> RecordedWaves;
};
