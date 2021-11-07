#include "Observation.h"

void UObservation::Initialise(AGridCell* GridCellArg)
{
	GridCell = GridCellArg;

	if (GridCellArg)
		OldWave = GridCellArg->Wave;
}
