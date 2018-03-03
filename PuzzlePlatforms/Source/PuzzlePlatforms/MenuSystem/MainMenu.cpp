// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "UObject/ConstructorHelpers.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/EditableText.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "ServerRow.h"


UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenCreateServerMenu);

	if (!ensure(CreateServerButton != nullptr)) return false;
	CreateServerButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(JoinServerButton != nullptr)) return false;
	JoinServerButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!ensure(CancelJoinButton != nullptr)) return false;
	CancelJoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(CancelCreateButton != nullptr)) return false;
	CancelCreateButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	return true;
}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

void UMainMenu::JoinServer()
{
	if (MenuInterface != nullptr && SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index = %d"), SelectedIndex.GetValue())
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index not set"))
	}
		//if (!ensure( IPAddressField != nullptr)) return;
		//const FString& Address = IPAddressField->GetText().ToString();
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;
		
		Row->ServerName->SetText(FText::FromString(ServerData.Name));
		Row->HostName->SetText(FText::FromString(ServerData.HostUsername));
		FString Players = FString::FromInt(ServerData.CurentPlayers) 
									+ "/" + FString::FromInt(ServerData.MaxPlayers);
		Row->Players->SetText(FText::FromString(Players));
		Row->Setup(this, i);
		++i;

		ServerList->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		UServerRow* Row = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(ServerList != nullptr)) return;
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
}

void UMainMenu::OpenCreateServerMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(CreateServerMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(CreateServerMenu);
	if (MenuInterface != nullptr)
	{
		
	}
}

void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->GetFirstPlayerController()->ConsoleCommand("quit");
}

void UMainMenu::SetMenuInterface(IMenuInterface* Interface)
{
	if (!ensure(Interface != nullptr)) return;

	MenuInterface = Interface;
}

void UMainMenu::Setup()
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

void UMainMenu::Teardown()
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