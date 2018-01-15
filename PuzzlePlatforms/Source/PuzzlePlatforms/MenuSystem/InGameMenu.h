// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuInterface(IMenuInterface* MenuInterface);

	void Setup();

	void Teardown();
	
protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void QuitToMainMenu();

	UFUNCTION()
	void QuitGame();

	UFUNCTION()
	void CancelButtonPressed();

	IMenuInterface* MenuInterface;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenuButton;
};
