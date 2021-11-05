#pragma once

#include "CoreMinimal.h"
#include "GridCell.h"
#include "Tile.h"
#include "Propagation.h"
#include "Observation.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API UObservation : public UObject
{
	GENERATED_BODY()

public:
	void Initialise(AGridCell* GridCellArg);

	AGridCell* GridCell;

	TArray<TSubclassOf<ATile>> OldWave;

	TArray<UPropagation*> Propagations;
};
