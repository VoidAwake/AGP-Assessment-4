// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerHUD.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter.generated.h"


class APlayerCharacter;
USTRUCT()
struct FPlayerStat
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Kills;
	
	UPROPERTY()
	int32 Deaths;

	UPROPERTY()
	APlayerCharacter* Owner;
	
	FPlayerStat()
	{
		Kills = 0;
		Deaths = 0;
		Owner = nullptr;
	}

	void SetOwner(APlayerCharacter* newOwner)
	{
		Owner = newOwner;	
	}

	void SetDeaths(int32 amount)
	{
		Deaths = amount;
	}

	void SetKills(int32 amount)
	{
		Kills = amount;
	}
	
};

UCLASS()
class ADVGAMESPROGRAMMING_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float SprintMovementSpeed;
	float NormalMovementSpeed;

public:	

	UPROPERTY(EditAnywhere)
	float LookSensitivity;
	UPROPERTY(EditInstanceOnly)
	float SprintMultiplier;

	UPROPERTY(EditAnywhere)
	FPlayerStat Stat;
	
	class UHealthComponent* HealthComponent;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void Strafe(float Value);
	void LookUp(float Value);
	void Turn(float Value);
	void SprintStart();
	void SprintEnd();
	void Reload();

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintReload();

	UFUNCTION(Server, Reliable)
	void ServerSprintStart();
	UFUNCTION(Server, Reliable)
	void ServerSprintEnd();
	UFUNCTION(Client, Reliable)
	void SetPlayerHUDVisibility(bool bHUDVisible);

	void OnDeath();

	UFUNCTION(BlueprintCallable)
	void IncrementKills();

	UFUNCTION(BlueprintCallable)
	void IncrementDeaths();
	
private:

	UCameraComponent* Camera;
	class UFirstPersonAnimInstance* AnimInstance;

	public:

	void SetGravity(float amount) const;

	bool FlagStartGravityPush;
	
	void PushPlayer(FVector Velocity, bool VelocityChange) const;

	UPROPERTY(EditAnywhere)
	bool IsAffectedByGravityField;

	UPROPERTY(EditAnywhere)
	bool CanBeAffectedByGravityField;

	UPROPERTY(EditAnywhere)
	class ULerp* LerpComponent;

	bool IsLerpable;
};
