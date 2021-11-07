#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	LEFT,
	RIGHT,
	FORWARD,
	BACK,
	UP,
	DOWN,
};
