// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GravityObject.generated.h"

UCLASS()
class ADVGAMESPROGRAMMING_API AGravityObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	
	float radius;

	bool FlagStartGravityPush;
	



};
