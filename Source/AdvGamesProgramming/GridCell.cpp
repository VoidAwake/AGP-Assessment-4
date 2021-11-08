#include "GridCell.h"
#include "Engine/World.h"
#include "DirectionUtility.h"

void UGridCell::Observe()
{
	//RecordWave();

	// Pick a random TileType from the wave
	TSubclassOf<ATile> TileTypeToSpawn = Wave[FMath::RandRange(0, Wave.Num() - 1)];

	CreateTile(TileTypeToSpawn);
}

//bool UGridCell::RestoreWave()
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

void UGridCell::Clear()
{
	if (Tile)
		Tile->Destroy();
}

void UGridCell::CreateTile(TSubclassOf<ATile> TileTypeToSpawn)
{
	Clear();

	Wave = { TileTypeToSpawn };

	if (!Parent) {
		UE_LOG(LogTemp, Warning, TEXT("Tile Creation Failed: No parent to attach tile to."));
		return;
	}

	UWorld* World = Parent->GetWorld();

	if (!World) {
		UE_LOG(LogTemp, Warning, TEXT("Tile Creation Failed: Could not find world."));
		return;
	}
	
	Tile = World->SpawnActor<ATile>(TileTypeToSpawn, WorldPosition, FRotator::ZeroRotator);

	Tile->AttachToActor(Parent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false));

	Tile->ForceNetUpdate();

	//UE_LOG(LogTemp, Warning, TEXT("Cell observed"));
}

void UGridCell::Initialise(AActor* ParentArg, TArray<TSubclassOf<ATile>> TileSet, FVector WorldPositionArg, int x, int y, int z)
{
	Initialise(ParentArg, TileSet);

	WorldPosition = WorldPositionArg;

	GridPosition = FVector(x, y, z);
}

void UGridCell::Initialise(AActor* ParentArg, TArray<TSubclassOf<ATile>> TileSet)
{
	Wave.Empty();

	for (auto TileType : TileSet) {
		Wave.Add(TileType);
	}

	Parent = ParentArg;
}

bool UGridCell::Allows(TSubclassOf<ATile> NeighbourTileType, EDirection Direction)
{
	EDirection Opposite = DirectionUtility::OppositeDirection(Direction);

	EFaceTag NeighbourFaceTag = NeighbourTileType->GetDefaultObject<ATile>()->FaceTagInDirection(Opposite);

	for (auto TileType : Wave) {
		if (TileType->GetDefaultObject<ATile>()->FaceTagInDirection(Direction) == NeighbourFaceTag)
			return true;
	}

	return false;
}

bool UGridCell::RemoveTileTypesFromWave(TArray<TSubclassOf<ATile>> TileTypesToRemove) {
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

//void UGridCell::RecordWave()
//{
//	UPropagation* WaveRecord = NewObject<UPropagation>(this);
//
//	WaveRecord->Initialise(this);
//
//	RecordedWaves.Push(WaveRecord);
//}
