// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GravityShooterCharacter.h"
#include "Components/ActorComponent.h"
#include "Lerp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVGAMESPROGRAMMING_API ULerp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULerp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	 void LerpFVector(AActor* Actor, FVector StartPosition, FVector EndPosition, float Duration);

	UFUNCTION()
	void LerpFVector(APawn* Player, FVector StartPosition, FVector EndPosition, float Duration);
	
	bool StartLerp;

	bool IsLerping;

	UPROPERTY(VisibleAnywhere)
	FVector StartPosition;

	UPROPERTY(VisibleAnywhere)
	FVector EndPosition;

	UPROPERTY(VisibleAnywhere)
	FVector CurrentPosition;

	UPROPERTY(VisibleAnywhere)
	float LerpDuration;

	UPROPERTY(VisibleAnywhere)
	float TimeElapsed;


	UPROPERTY(VisibleAnywhere)
	AActor* AffectedActor;

	UPROPERTY(VisibleAnywhere)
	APawn* AffectedPlayer;

	void Delay(float DeltaTime);

	void StartDelay(float TimeOfDelay);

	float TimeOfDelay;

	float DelayElapsedTime;

	bool DoDelay;
		
};
