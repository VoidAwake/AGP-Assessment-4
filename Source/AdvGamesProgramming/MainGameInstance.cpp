// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

UMainGameInstance::UMainGameInstance(const FObjectInitializer& ObjectInitialize) {
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetObject(TEXT("/Game/Widgets/MainMenuWidget"));

	MainMenuWidgetClass = MainMenuWidgetObject.Class;
}

void UMainGameInstance::LoadMenu()
{
	Menu = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);

	if (!Menu)
		return;

	Menu->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(Menu->TakeWidget());

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PlayerController)
		return;

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void UMainGameInstance::Init()
{
	Subsystem = IOnlineSubsystem::Get();

	if (Subsystem) {
		UE_LOG(LogTemp, Warning, TEXT("Found Online Subsystem: %s"), *Subsystem->GetSubsystemName().ToString())

		SessionInterface = Subsystem->GetSessionInterface();

		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnCreateSessionComplete);

		CreateSession(TEXT("Test Session"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Unable to find Online Subsystem"))
	}
}

void UMainGameInstance::CreateSession(FName SessionName)
{
	if (!SessionInterface.IsValid())
		return;

	FOnlineSessionSettings SessionSettings;

	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 3;
	SessionSettings.bShouldAdvertise = true;

	SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
		UE_LOG(LogTemp, Warning, TEXT("Session Created Successfully"))
	else
		UE_LOG(LogTemp, Warning, TEXT("Session was not Created"))
}
