// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData 
{
	GENERATED_BODY()

	FString Name;
	uint16 CurentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainMenu(const FObjectInitializer & ObjectInitializer);

	void SetMenuInterface(IMenuInterface* Interface);

	void SetServerList(TArray<FServerData> ServerNames);

	void Setup();

	void Teardown();

	void SelectIndex(uint32 Index);

	UPROPERTY(meta = (BindWidget))
	class UEditableText* ServerNameInput;
	
protected:
	virtual bool Initialize() override;

private:
	TSubclassOf<class UUserWidget> ServerRowClass;

	IMenuInterface* MenuInterface;

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinServerButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateServerButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelCreateButton;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* CreateServerMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenCreateServerMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void QuitGame();

	TOptional<uint32> SelectedIndex;
	
	void UpdateChildren();
};
