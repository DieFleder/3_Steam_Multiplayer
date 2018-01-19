// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzelPlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTrigger.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionInterface.h"

#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include "Kismet/GameplayStatics.h"

const static FName SESSION_NAME = TEXT("My Session Game");

UPuzzelPlatformsGameInstance::UPuzzelPlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MainMenuBPClass.Class != nullptr)) return;

	MainMenuClass = MainMenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;

	InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzelPlatformsGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem Name: %s"), *Subsystem->GetSubsystemName().ToString())
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzelPlatformsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzelPlatformsGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzelPlatformsGameInstance::OnFindSessionsComplete);

			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			if (SessionSearch.IsValid())
			{
				SessionSearch->bIsLanQuery = true;
				SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
			}
		}
	}
}

void UPuzzelPlatformsGameInstance::LoadMainMenu()
{
	if (!ensure(MainMenuClass != nullptr)) return;

	MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
	if (!ensure(MainMenu != nullptr)) return;

	MainMenu->Setup();
	MainMenu->SetMenuInterface(this);
}

void UPuzzelPlatformsGameInstance::LoadInGameMenu()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuClass);
	if (!ensure(InGameMenu != nullptr)) return;
	
	InGameMenu->Setup();
	InGameMenu->SetMenuInterface(this);
}

void UPuzzelPlatformsGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UPuzzelPlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Failed"))
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Called"))
	UE_LOG(LogTemp, Warning, TEXT("%s Successfully created"), *SessionName.ToString())
	if (MainMenu != nullptr)
	{
		MainMenu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
}

void UPuzzelPlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}

void UPuzzelPlatformsGameInstance::Join(const FString& Address)
{
	if (MainMenu != nullptr)
	{
		MainMenu->Teardown();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzelPlatformsGameInstance::QuitToMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	PlayerController->ClientTravel("/Game/MenuSystem/LoadMenu", ETravelType::TRAVEL_Absolute);
}

void UPuzzelPlatformsGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UPuzzelPlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid())
	{
		if (SessionSearch->SearchResults.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("0 sessions found"))
			return;
		}
		else
		{
			for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
			{
				UE_LOG(LogTemp, Warning, TEXT("Session: %s was found"), *Result.GetSessionIdStr())
			}
		}
	}
}