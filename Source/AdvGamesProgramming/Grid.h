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

	UGridCell* GetGridCell(int x, int y, int z);

	void Clear();

	void ForEachGridCell(TFunctionRef<void(UGridCell*)> Func);

	void ForEachGridCell(TFunctionRef<void(UGridCell*, int, int, int)> Func);

	void GenerateGrid(TArray<TSubclassOf<ATile>> TileSet);

	UGridCell* GetAdjacentCell(int x, int y, int z, EDirection Direction);

	UGridCell* GetAdjacentCell(UGridCell* GridCell, EDirection Direction);

	TArray<UGridCell*> CreateBorders(TSubclassOf<ATile> BorderTile);

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
	bool bCentreOrigin;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATile> NewBorderTile;

	UPROPERTY(EditAnywhere)
	bool bClearGrid;

	void ClearGridCells(TArray<TSubclassOf<ATile>> TileSet);

	bool SizeChanged();

	void TryCreateBorderTile(TArray<UGridCell*>& ChangedGridCells, TSubclassOf<ATile> BorderTile, int x, int y, int z);

	UPROPERTY()
	TArray<UGridCell*> GridCells;

	int CurrentWidth;

	int CurrentDepth;

	int CurrentHeight;

	float CurrentTileSize;

	bool bCurrentCentreOrigin;
};
