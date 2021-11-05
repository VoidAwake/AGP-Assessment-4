#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridCell.h"
#include "Direction.h"
#include "Grid.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVGAMESPROGRAMMING_API UGrid : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UGrid();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AGridCell* GetGridCell(int x, int y, int z);

	void Clear();

	void ForEachGridCell(TFunctionRef<void(AGridCell*)> Func);

	void ForEachGridCell(TFunctionRef<void(AGridCell*, int, int, int)> Func);

	void GenerateGrid(TArray<TSubclassOf<ATile>> TileSet);

	AGridCell* GetAdjacentCell(int x, int y, int z, EDirection Direction);

	AGridCell* GetAdjacentCell(AGridCell* GridCell, EDirection Direction);

	TArray<AGridCell*> CreateBorders(TSubclassOf<ATile> BorderTile);

private:
	UPROPERTY(EditAnywhere)
	int Width;

	UPROPERTY(EditAnywhere)
	int Depth;

	UPROPERTY(EditAnywhere)
	int Height;

	UPROPERTY(EditAnywhere)
	float TileSize;

	UPROPERTY(EditAnywhere)
	bool bClearGrid;

	void ClearGridCells(TArray<TSubclassOf<ATile>> TileSet);

	bool SizeChanged();

	void TryCreateBorderTile(TArray<AGridCell*>& ChangedGridCells, TSubclassOf<ATile> BorderTile, int x, int y, int z);

	TArray<AGridCell*> GridCells;

	int CurrentWidth;

	int CurrentDepth;

	int CurrentHeight;

	float CurrentTileSize;
};
