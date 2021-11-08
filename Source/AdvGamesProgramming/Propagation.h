#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tile.h"
#include "GridCell.h"
#include "Propagation.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API UPropagation : public UObject
{
	GENERATED_BODY()

public:
	//void Initialise(UGridCell* GridCellArg);

	//UGridCell* GridCell;

	TArray<TSubclassOf<ATile>> OldWave;
};
