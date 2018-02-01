// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerRow.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "MainMenu.h"


void UServerRow::Setup(UMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;

	if (!ensure(ServerRowButton != nullptr)) return;
	ServerRowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	Parent->SelectIndex(Index);
}