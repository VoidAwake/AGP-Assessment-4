#include "Observation.h"

void UObservation::Initialise(UGridCell* GridCellArg)
{
	GridCell = GridCellArg;

	if (GridCellArg)
		OldWave = GridCellArg->Wave;
}
