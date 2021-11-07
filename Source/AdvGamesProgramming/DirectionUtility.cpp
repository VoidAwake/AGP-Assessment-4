#include "DirectionUtility.h"

FString DirectionUtility::DirectionToString(EDirection Direction)
{
	switch (Direction)
	{
		case EDirection::LEFT:
			return FString(TEXT("Left"));
		case EDirection::RIGHT:
			return FString(TEXT("Right"));
		case EDirection::FORWARD:
			return FString(TEXT("Forward"));
		case EDirection::BACK:
			return FString(TEXT("Back"));
		case EDirection::UP:
			return FString(TEXT("Up"));
		case EDirection::DOWN:
			return FString(TEXT("Down"));
	}

	return FString();
}

EDirection DirectionUtility::OppositeDirection(EDirection Direction)
{
	switch (Direction) {
		case EDirection::RIGHT:
			return EDirection::LEFT;
		case EDirection::LEFT:
			return EDirection::RIGHT;
		case EDirection::BACK:
			return EDirection::FORWARD;
		case EDirection::FORWARD:
			return EDirection::BACK;
		case EDirection::DOWN:
			return EDirection::UP;
		case EDirection::UP:
			return EDirection::DOWN;
	}

	return EDirection();
}
