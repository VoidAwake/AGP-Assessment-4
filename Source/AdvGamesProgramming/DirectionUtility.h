#pragma once

#include "CoreMinimal.h"
#include "Direction.h"

static class ADVGAMESPROGRAMMING_API DirectionUtility
{
public:
	static FString DirectionToString(EDirection Direction);
	
	static EDirection OppositeDirection(EDirection Direction);
};
