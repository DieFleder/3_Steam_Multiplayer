// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameMenu.h"

#include "Components/Button.h"


bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(MainMenuButton != nullptr)) return false;
	MainMenuButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitToMainMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitGame);

	if (!ensure(CancelButton != nullptr)) return false;
	CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelButtonPressed);

	return true;
}

void UInGameMenu::Setup()
{
	this->AddToViewport();

	FInputModeUIOnly InputMode = FInputModeUIOnly();
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(this->TakeWidget());

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void UInGameMenu::Teardown()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameOnly InputMode = FInputModeGameOnly();

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = false;
}

void UInGameMenu::SetMenuInterface(IMenuInterface* Interface)
{
	if (!ensure(Interface != nullptr)) return;
	MenuInterface = Interface;
}

void UInGameMenu::CancelButtonPressed()
{
	Teardown();
}

void UInGameMenu::QuitToMainMenu()
{
	Teardown();
	MenuInterface->QuitToMainMenu();
}

void UInGameMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->GetFirstPlayerController()->ConsoleCommand("quit");
}