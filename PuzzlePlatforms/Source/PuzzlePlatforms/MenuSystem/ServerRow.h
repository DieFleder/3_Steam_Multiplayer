// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Players;

	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* ServerIndex;
	
	void Setup(class UMainMenu* Parent, uint32 Index);
	

private:
	UPROPERTY()
	UMainMenu* Parent;

	uint32 Index;

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerRowButton;

	UFUNCTION()
	void OnClicked();
};
