// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGameMode.h"
#include "PuzzlePlatformsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

#include "PuzzelPlatformsGameInstance.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++PlayerCount;

	if (PlayerCount >= MaxPlayers)
	{
		GetWorldTimerManager().SetTimer(StartGameTimer, this, &ALobbyGameMode::CountDown, 1.0f, true, 2.0f);
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--PlayerCount;
}

void ALobbyGameMode::CountDown()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting game in: %d seconds"), StartCount)
	--StartCount;

	if (StartCount <= 0)
	{
		GetWorldTimerManager().ClearTimer(StartGameTimer);

		auto GameInstance = Cast<UPuzzelPlatformsGameInstance>(GetGameInstance());

		GameInstance->StartSession();

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		AGameModeBase::bUseSeamlessTravel = true;
		World->ServerTravel("/Game/PuzzelPlatforms/Maps/Game?listen");
	}
}