// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "FirstPersonAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "HealthComponent.h"
#include "Lerp.h"
#include "MultiplayerGameMode.h"
#include "Engine/World.h"
#include "PlayerHUD.h"
#include "GameFramework/HUD.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	//Set default member variable values
	LookSensitivity = 1.0f;
	SprintMultiplier = 1.5f;

	SprintMovementSpeed = GetCharacterMovement()->MaxWalkSpeed * SprintMultiplier;
	NormalMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Initialise the camera variable
	Camera = FindComponentByClass<UCameraComponent>();
	//Initialise the health component
	HealthComponent = FindComponentByClass<UHealthComponent>();
	//UE_LOG(LogTemp, Warning, TEXT("IM HERE"))
	if (HealthComponent)
		HealthComponent->SetIsReplicated(true);

	// Get the skeletal mesh and then get the anim instance from it cast to the first person anim instance.
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("Arms")));
	if (SkeletalMesh) // Make sure the skeletal mesh was found
	{
		AnimInstance = Cast<UFirstPersonAnimInstance>(SkeletalMesh->GetAnimInstance());
	}

	LerpComponent = FindComponentByClass<ULerp>();
	FlagStartGravityPush = false;
	CanBeAffectedByGravityField = true;
	IsLerpable = true;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &APlayerCharacter::Strafe);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &APlayerCharacter::SprintEnd);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Reload);
}

void APlayerCharacter::MoveForward(float Value) 
{
	
	FRotator ForwardRotation = GetControlRotation();
	ForwardRotation.Roll = 0.0f;
	ForwardRotation.Pitch = 0.0f;
	AddMovementInput(ForwardRotation.Vector(), Value);
}

void APlayerCharacter::Strafe(float Value) 
{
	AddMovementInput(GetActorRightVector(), Value);
}

void APlayerCharacter::LookUp(float Value) 
{

	FRotator LookUpRotation = FRotator::ZeroRotator;
	LookUpRotation.Pitch = Value * LookSensitivity;
	if (Camera)
	{
		if (Camera->RelativeRotation.Pitch + LookUpRotation.Pitch < 90.0f
			&& Camera->RelativeRotation.Pitch + LookUpRotation.Pitch > -90.0f)
		{
			Camera->AddRelativeRotation(LookUpRotation);
			Camera->RelativeRotation.Yaw = 0.0f;
			Camera->RelativeRotation.Roll = 0.0f;
		}
	}
}

void APlayerCharacter::Turn(float Value) 
{
	AddControllerYawInput(Value * LookSensitivity);
}

void APlayerCharacter::SprintStart()
{
	ServerSprintStart();
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;

	if (AnimInstance)
	{
		AnimInstance->bIsSprinting = true;
	}
}

void APlayerCharacter::SprintEnd()
{
	ServerSprintEnd();
	GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;

	if (AnimInstance)
	{
		AnimInstance->bIsSprinting = false;
	}
}

void APlayerCharacter::ServerSprintStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
}

void APlayerCharacter::ServerSprintEnd_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
}

void APlayerCharacter::Reload()
{
	BlueprintReload();
}

void APlayerCharacter::OnDeath()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AMultiplayerGameMode* MultiplayerGameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode());
		if (MultiplayerGameMode)
		{
			MultiplayerGameMode->Respawn(GetController());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to find the GameMode"))
		}
	}
}

void APlayerCharacter::PushPlayer(FVector Velocity, bool VelocityChange) const
{
	GetCharacterMovement()->AddImpulse(Velocity,VelocityChange);
}

void APlayerCharacter::SetGravity(float amount) const
{
	GetCharacterMovement()->GravityScale = amount;
}

void APlayerCharacter::SetPlayerHUDVisibility_Implementation(bool bHUDVisible)
{
	if (IsLocallyControlled())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD()))
			{
				bHUDVisible ? PlayerHUD->ShowHUD() : PlayerHUD->HideHUD();
				UE_LOG(LogTemp, Warning, TEXT("Hiding the HUD"))
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Can't find HUD on controller. AUTONOMOUS"))
			}
		}
	}
}


