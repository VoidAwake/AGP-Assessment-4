#include "Tile.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
}

void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

EFaceTag ATile::FaceTagInDirection(EDirection Direction)
{
	if (FaceTags.Contains(Direction))
		return FaceTags[Direction];
	else
		return EFaceTag::EMPTY;
}
