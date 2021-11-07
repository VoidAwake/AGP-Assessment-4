// Fill out your copyright notice in the Description page of Project Settings.


#include "Lerp.h"

#include "GravComponent.h"
#include "GravityObject.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
ULerp::ULerp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	IsLerping = false;
	StartLerp = false;

	// ...
}


// Called when the game starts
void ULerp::BeginPlay()
{
	Super::BeginPlay();

	DoDelay = false;
	DelayElapsedTime = 0.0f;
	TimeOfDelay = 0.0f;
	// ...
	
}

// Called every frame
void ULerp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DoDelay)
	{
		Delay(DeltaTime);
	}

	if (IsLerping && AffectedPlayer != nullptr)
	{
		// UE_LOG(LogTemp,Warning,TEXT("LerpStarted"));
		
		if (TimeElapsed < LerpDuration)
		{

			AffectedPlayer->SetActorLocation(FMath::Lerp(StartPosition,EndPosition,TimeElapsed / 
			LerpDuration));
			TimeElapsed += DeltaTime;

		} else
		{
			// auto character = Cast<AGravityShooterCharacter>(AffectedPlayer);
			//
			// if (character != nullptr)
			// {
			// 	UE_LOG(LogTemp,Warning,TEXT("LerpStarted"));
			// 	character->FlagStartGravityPush = true;
			// 	StartDelay(1.0f);
			// }
			//
			// IsLerping = false;
		}
	} else if (IsLerping && AffectedActor != nullptr)
	{
		if (TimeElapsed < LerpDuration)
		{

			AffectedActor->SetActorLocation(FMath::Lerp(StartPosition,EndPosition,TimeElapsed / 
			LerpDuration));
			TimeElapsed += DeltaTime;

		} else
		{
			UGravComponent* gravComponent = Cast<UGravComponent>(AffectedActor->GetComponentByClass(UGravComponent::StaticClass()));

			if (gravComponent != nullptr)
			{
				AGravityObject* GravObject = Cast<AGravityObject>(AffectedActor);

				GravObject->FlagStartGravityPush = true;
				
				StartDelay(1.0f);
			}

			IsLerping = false;
		}
	}
		
	// ...
}

void ULerp::LerpFVector(AActor* Actor, const FVector Start, const FVector End, const float 
Duration)
{

	
	UGravComponent* gravComponent = Cast<UGravComponent>(Actor->GetComponentByClass(UGravComponent::StaticClass()));
	
	if (IsLerping || !gravComponent->IsLerpable)
		return;

	
	AffectedActor = Actor;
	StartPosition = Start;
	EndPosition = End;
	TimeElapsed = 0;
	LerpDuration = Duration;
	IsLerping = true;
	
	USphereComponent* SphereComponent = Cast<USphereComponent>(Actor->GetComponentByClass(USphereComponent::StaticClass()));
	
	if (SphereComponent != nullptr)
	{
		SphereComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector,false,NAME_None);
		SphereComponent->SetPhysicsLinearVelocity(FVector::ZeroVector,false,NAME_None);
	
	}

	gravComponent->IsLerpable = false;
	
}

void ULerp::LerpFVector(APawn* Player,  FVector Start,  FVector End, const float Duration)
{
	//auto character = Cast<AGravityShooterCharacter>(Player);

	// if (IsLerping || !character->IsLerpable)
	// 	return;
	//
	// UE_LOG(LogTemp,Warning,TEXT("LERPSETUP"));
	//
	// AffectedPlayer = Player;
	// StartPosition = Start;
	// EndPosition = End;
	// TimeElapsed = 0.0f;
	// LerpDuration = Duration;
	// IsLerping = true;


	// character->GetCharacterMovement()->StopMovementImmediately();
	// character->IsLerpable = false;

	
	//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, AffectedPlayer->GetName());

}

void ULerp::Delay(float DeltaTime)
{
	//auto character = Cast<AGravityShooterCharacter>(AffectedPlayer);

	// if (character != nullptr)
	// {
	// 	if (DelayElapsedTime < TimeOfDelay)
	// 	{
	// 		UE_LOG(LogTemp,Warning,TEXT("isdelayed"));
	// 		DelayElapsedTime += DeltaTime;
	// 	} else
	// 	{
	// 		UE_LOG(LogTemp,Warning,TEXT("delayedcomplete"));
	// 		character->IsLerpable = true;
	// 		DoDelay = false;
	// 		DelayElapsedTime = 0;
	// 	}
	// } else
	// {
	// 	if (DelayElapsedTime < TimeOfDelay)
	// 	{
	// 		UE_LOG(LogTemp,Warning,TEXT("isdelayed"));
	// 		DelayElapsedTime += DeltaTime;
	// 	} else
	// 	{
	// 		UE_LOG(LogTemp,Warning,TEXT("delayedcomplete"));
	// 		UGravComponent* gravComponent = Cast<UGravComponent>(AffectedActor->GetComponentByClass(UGravComponent::StaticClass()));
	// 		gravComponent->IsLerpable = false;
	// 		DoDelay = false;
	// 		DelayElapsedTime = 0;
	// 	}
	// }
}

void ULerp::StartDelay(float timeOfDelay)
{
	this->TimeOfDelay = timeOfDelay;
	DoDelay = true;
}

