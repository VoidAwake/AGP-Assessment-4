#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Tile.h"
#include "TileSet.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API UTileSet : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ATile>> TileSet;
};
