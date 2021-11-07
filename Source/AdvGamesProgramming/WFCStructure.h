#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Grid.h"
#include "GridCell.h"
#include "Containers/Queue.h"
#include "TileSet.h"
#include "Observation.h"
#include "WFCStructure.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AWFCStructure : public AActor
{
	GENERATED_BODY()
	
public:	
	AWFCStructure();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldTickIfViewportsOnly() const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerGenerate();

	UFUNCTION()
	void Generate();

	UPROPERTY(ReplicatedUsing = Generate)
	int32 Seed;

private:
	UPROPERTY(EditAnywhere)
	UTileSet* TileSet;

	UPROPERTY(EditAnywhere)
	bool bCleanBorders;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ATile> BorderTile;

	UPROPERTY(EditAnywhere)
	int MaxGenerationAttempts;

	UPROPERTY(EditAnywhere)
	bool bStopOnContradiction;

	UPROPERTY(EditAnywhere)
	bool bRegenerate;

	UPROPERTY(EditAnywhere, Category="Dev Tools")
	bool bGenerateGrid;

	UPROPERTY(EditAnywhere, Category = "Dev Tools")
	bool bCreateBorders;

	UPROPERTY(EditAnywhere, Category = "Dev Tools")
	bool bTestObserve;

	UPROPERTY(EditAnywhere, Category = "Dev Tools")
	bool bTestPropagate;

	UPROPERTY(EditAnywhere, Category = "Dev Tools")
	bool bTestBacktrack;

	UPROPERTY(VisibleInstanceOnly)
	UGrid* Grid;

	int Observe();

	AGridCell* GetCellWithMinEntropy();

	bool Propagate(bool bEnableBacktrack = true);

	bool RemoveDisallowedTileTypes(AGridCell* GridCell, AGridCell* AdjacentGridCell, EDirection Direction);

	void CreateBorders();

	//bool Backtrack();

	TQueue<AGridCell*> ChangedCellsQueue;

	TArray<EDirection> Directions;

	TArray<UObservation*> BacktrackQueue;
};
