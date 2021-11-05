#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Direction.h"
#include "FaceTag.h"
#include "Tile.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	EFaceTag FaceTagInDirection(EDirection Direction);

private:
	UPROPERTY(EditAnywhere)
	TMap<EDirection, EFaceTag> FaceTags;
};
