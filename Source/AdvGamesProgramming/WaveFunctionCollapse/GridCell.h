#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Direction.h"
#include "Propagation.h"
#include "GridCell.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AGridCell : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridCell();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TArray<TSubclassOf<ATile>> Wave;

	void Initialise(TArray<TSubclassOf<ATile>> TileSet, int x, int y, int z);

	void Initialise(TArray<TSubclassOf<ATile>> TileSet);

	bool Allows(TSubclassOf<ATile> NeighbourTileType, EDirection Direction);

	void Observe();

	//bool RestoreWave();

	void CreateTile(TSubclassOf<ATile> TileTypeToSpawn);

	void Clear();

	bool RemoveTileTypesFromWave(TArray<TSubclassOf<ATile>> TileTypesToRemove);

	//void RecordWave();

	FVector GridPosition;

	ATile* Tile;

private:
	//TArray<UPropagation*> RecordedWaves;
};
