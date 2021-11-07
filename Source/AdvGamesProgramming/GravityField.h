// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GravityShooterCharacter.h"
#include "Lerp.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GravityField.generated.h"

UENUM()
enum class EGravityFieldType : uint8
{
	VERTICAL_PUSH,
	VERTICAL_PULL,
	HORIZONTAL_PUSH_X,
	HORIZONTAL_PULL_X,
	HORIZONTAL_PUSH_Y,
	HORIZONTAL_PULL_Y
};

UENUM()
enum class EGravityFieldTypeSimplified : uint8
{
	PUSH,
	PULL
};

UENUM()
enum class EGravityDirection : uint8
{
	Positive_X,
	Negative_X,
	Positive_Y,
	Negative_Y,
	Positive_Z,
	Negative_Z,
};




USTRUCT()
struct FGravityFieldGenerator
{

	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float MaxDistance;

	UPROPERTY(EditAnywhere)
	float GravityStrength;
	
	UPROPERTY(EditAnywhere)
	EGravityDirection Direction;

	UPROPERTY(EditAnywhere)
	FColor Color;
	
	UPROPERTY(EditAnywhere)
	EGravityFieldTypeSimplified GravityType;

	UPROPERTY(VisibleAnywhere)
	EGravityFieldType GravityFieldType;

	UPROPERTY(EditAnywhere)
	bool StopOnActor;
	

	void CalculateFieldType()
	{
		switch(Direction)
		{
		case EGravityDirection::Positive_X:
			GravityFieldType = GravityType == EGravityFieldTypeSimplified::PULL ? EGravityFieldType::HORIZONTAL_PULL_X : EGravityFieldType::HORIZONTAL_PUSH_X;
			break;
		case EGravityDirection::Negative_X:
			GravityFieldType = GravityType == EGravityFieldTypeSimplified::PULL ? EGravityFieldType::HORIZONTAL_PULL_X : EGravityFieldType::HORIZONTAL_PUSH_X;
			break;
		case EGravityDirection::Positive_Y:
			GravityFieldType = GravityType == EGravityFieldTypeSimplified::PULL ? EGravityFieldType::HORIZONTAL_PULL_Y : EGravityFieldType::HORIZONTAL_PUSH_Y;
			break;
		case EGravityDirection::Negative_Y:
			GravityFieldType = GravityType == EGravityFieldTypeSimplified::PULL ? EGravityFieldType::HORIZONTAL_PULL_X : EGravityFieldType::HORIZONTAL_PUSH_X;
			break;
		case EGravityDirection::Positive_Z:
			GravityFieldType = GravityType == EGravityFieldTypeSimplified::PULL ? EGravityFieldType::VERTICAL_PULL : EGravityFieldType::VERTICAL_PUSH;
			break;
		case EGravityDirection::Negative_Z:
			GravityFieldType = GravityType == EGravityFieldTypeSimplified::PULL ? EGravityFieldType::VERTICAL_PULL : EGravityFieldType::VERTICAL_PUSH;
			break;
		default: ;
		}
	}

	UPROPERTY(EditAnywhere)
	int32 AllowedActorsAtOnce;

	FGravityFieldGenerator();
};


UCLASS()
class ADVGAMESPROGRAMMING_API AGravityField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityField();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldTickIfViewportsOnly() const override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult) ;
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) ;

	UFUNCTION(BlueprintCallable)
	void CreateGravityField();

	void GenerateGravityField();
	
	static void OnEnter(AActor* InteractedActor);

	UPROPERTY(EditAnywhere)
	UBoxComponent* GravityBoxComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GravityMeshComponent;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> AffectedActors;

	UPROPERTY(VisibleAnywhere)
	TArray<APlayerCharacter*> AffectedPlayers;

	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> Materials;


	UPROPERTY(EditAnywhere, Category = "Create Gravity Field")
	float Height;

	UPROPERTY(EditAnywhere, Category = "Create Gravity Field")
	float Width;

	UPROPERTY(EditAnywhere, Category = "Create Gravity Field")
	float Length;

	UPROPERTY(EditAnywhere, Category = "Create Gravity Field", meta=(UIMin = "0", UIMax = "500"))
	float GravityStrength;

	
	UPROPERTY(EditAnywhere, Category = "Create Gravity Field")
	bool DontUpdateSizeAndStrengthParameters;


	UPROPERTY(EditAnywhere, Category= "Create Gravity Field")
	EGravityFieldType GravityType;
	
	UPROPERTY(EditAnywhere, Category= "Create Gravity Field", meta=(UIMin = "0", UIMax = "5000"))
	int32 ActorsAllowedAtOnce;

	UPROPERTY(EditAnywhere, Category= "Create Gravity Field")
	bool bCreateGravityField;

	UPROPERTY(EditAnywhere, Category= "Generate Gravity Field")
	bool bGenerateGravityField;

	UPROPERTY(EditAnywhere, Category =" Generate Gravity Field")
	FGravityFieldGenerator GravityFieldGenerator;
	
	private:

	void DeterminePushForPlayer(APlayerCharacter* character);

	void DetermineLerpPositionForActor(AActor* actor,ULerp* actorLerpComponent);

	void DetermineLerpPositionForPlayer(APlayerCharacter* player);
	
	void DeterminePushForActor(UStaticMeshComponent* MeshComp);
	
	
};


