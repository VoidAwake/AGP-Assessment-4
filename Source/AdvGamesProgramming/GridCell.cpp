#include "GridCell.h"
#include "Engine/World.h"
#include "DirectionUtility.h"

AGridCell::AGridCell()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));

	bReplicates = true;
}

void AGridCell::BeginPlay()
{
	Super::BeginPlay();
}

void AGridCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridCell::Observe()
{
	//RecordWave();

	// Pick a random TileType from the wave
	TSubclassOf<ATile> TileTypeToSpawn = Wave[FMath::RandRange(0, Wave.Num() - 1)];

	CreateTile(TileTypeToSpawn);
}

//bool AGridCell::RestoreWave()
//{
//	TArray<TSubclassOf<ATile>> OldWave = RecordedWaves.Pop()->OldWave;
//
//	if (Wave.Num() == 1) {
//		Clear();
//
//		TSubclassOf<ATile> ObservedTile = Wave[0];
//
//		Wave.Empty();
//
//		for (auto TileType : OldWave) {
//			if (TileType != ObservedTile) // Don't add the tile that caused the contradiction
//				Wave.Add(TileType);
//		}
//	}
//	else {
//		Wave.Empty();
//
//		for (auto TileType : OldWave) {
//			Wave.Add(TileType);
//		}
//	}
//
//	return Wave.Num() > 0;
//}

void AGridCell::Clear()
{
	if (Tile)
		Tile->Destroy();
}

void AGridCell::CreateTile(TSubclassOf<ATile> TileTypeToSpawn)
{
	Clear();

	Wave = { TileTypeToSpawn };
	
	Tile = GetWorld()->SpawnActor<ATile>(TileTypeToSpawn, GetActorLocation(), FRotator::ZeroRotator);

	Tile->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);

	Tile->ForceNetUpdate();

	//UE_LOG(LogTemp, Warning, TEXT("Cell observed"));
}

void AGridCell::Initialise(TArray<TSubclassOf<ATile>> TileSet, int x, int y, int z)
{
	Initialise(TileSet);

	GridPosition = FVector(x, y, z);
}

void AGridCell::Initialise(TArray<TSubclassOf<ATile>> TileSet)
{
	Wave.Empty();

	for (auto TileType : TileSet) {
		Wave.Add(TileType);
	}
}

bool AGridCell::Allows(TSubclassOf<ATile> NeighbourTileType, EDirection Direction)
{
	EDirection Opposite = DirectionUtility::OppositeDirection(Direction);

	EFaceTag NeighbourFaceTag = NeighbourTileType->GetDefaultObject<ATile>()->FaceTagInDirection(Opposite);

	for (auto TileType : Wave) {
		if (TileType->GetDefaultObject<ATile>()->FaceTagInDirection(Direction) == NeighbourFaceTag)
			return true;
	}

	return false;
}

bool AGridCell::RemoveTileTypesFromWave(TArray<TSubclassOf<ATile>> TileTypesToRemove) {
	if (TileTypesToRemove.Num() == 0)
		return true;

	//RecordWave();

	for (auto TileTypeToRemove : TileTypesToRemove) {
		Wave.Remove(TileTypeToRemove);
	}

	if (Wave.Num() == 0)
		return false;
	
	if (Wave.Num() == 1)
		Observe();

	return true;
}

//void AGridCell::RecordWave()
//{
//	UPropagation* WaveRecord = NewObject<UPropagation>(this);
//
//	WaveRecord->Initialise(this);
//
//	RecordedWaves.Push(WaveRecord);
//}
