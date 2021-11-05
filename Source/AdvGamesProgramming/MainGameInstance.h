// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.h"
#include "OnlineSubsystem.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ADVGAMESPROGRAMMING_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMainGameInstance(const FObjectInitializer& ObjectInitialize);

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	virtual void Init();

	void CreateSession(FName SessionName);
	
private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UMainMenuWidget* Menu;

	IOnlineSubsystem* Subsystem;

	IOnlineSessionPtr SessionInterface;

	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
};
