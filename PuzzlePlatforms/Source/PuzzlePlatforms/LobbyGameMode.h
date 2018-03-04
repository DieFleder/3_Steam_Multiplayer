// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzlePlatformsGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API ALobbyGameMode : public APuzzlePlatformsGameMode
{
	GENERATED_BODY()
	
private:

	uint16 PlayerCount = 0;

	uint16 MaxPlayers = 3;

	virtual void BeginPlay() override;

	void CountDown();

	FTimerHandle StartGameTimer;
public:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;
	
	uint16 StartCount = 5;
};
