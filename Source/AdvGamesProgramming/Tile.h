#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Direction.h"
#include "FaceTag.h"
#include "Tile.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API ATile : public AStaticMeshActor
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
