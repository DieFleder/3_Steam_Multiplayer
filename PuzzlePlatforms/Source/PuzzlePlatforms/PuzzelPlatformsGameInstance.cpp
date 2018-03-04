// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzelPlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTrigger.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "Components/EditableText.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"
#include "Kismet/GameplayStatics.h"

const static FName SESSION_NAME = TEXT("Game");

UPuzzelPlatformsGameInstance::UPuzzelPlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MainMenuBPClass.Class != nullptr)) return;

	MainMenuClass = MainMenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;

	InGameMenuClass = InGameMenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> LoadingBPClass(TEXT("/Game/PuzzelPlatforms/Maps/WBP_Loading"));
	if (!ensure(LoadingBPClass.Class != nullptr)) return;

	LoadScreenClass = LoadingBPClass.Class;
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
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzelPlatformsGameInstance::OnJoinSessionComplete);
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

void UPuzzelPlatformsGameInstance::LoadLoadScreen()
{
	if (!ensure(LoadScreenClass != nullptr)) return;

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

	World->ServerTravel("/Game/PuzzelPlatforms/Maps/Lobby?listen");

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

void UPuzzelPlatformsGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (MainMenu != nullptr)
	{
		MainMenu->Teardown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

}

void UPuzzelPlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString TravelURL;
	SessionInterface->GetResolvedConnectString(SessionName, TravelURL);

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
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
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.NumPublicConnections = 5;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;

		FString ServerName = MainMenu->ServerNameInput->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerName)

		SessionSettings.Set(TEXT("CustomServerName"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UPuzzelPlatformsGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UPuzzelPlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && MainMenu != nullptr)
	{
		
			TArray<FServerData> ServerNames;
			for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
			{
				FServerData Data;
				//Data.Name = Result.GetSessionIdStr();			
				Data.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
				Data.CurentPlayers = Data.MaxPlayers - Result.Session.NumOpenPublicConnections;
				Data.HostUsername = Result.Session.OwningUserName;
				FString TestSetting;
				if (Result.Session.SessionSettings.Get(TEXT("CustomServerName"), TestSetting))
				{
					UE_LOG(LogTemp, Warning, TEXT("Data found in settings: %s"), *TestSetting)
						Data.Name = TestSetting;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Didn't get expected data"))
				}
				ServerNames.Add(Data);
			}
			MainMenu->SetServerList(ServerNames);
	}
}

void UPuzzelPlatformsGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}