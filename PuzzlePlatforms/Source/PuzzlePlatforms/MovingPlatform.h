// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector MoveDirection;
	
	UPROPERTY(EditAnywhere, Category = "Setup")
	float Speed = 20;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float JourneyTolerance = 5;

	UPROPERTY(EditAnywhere, Category = "Setup", Meta = (MakeEditWidget = true) )
	FVector TargetLocation;

	void AddActiveTrigger();
	void RemoveActiveTrigger();


private:
	FVector GlobalTargetLocation;
	FVector GlobalStartLocation;
	float JourneyLength;

	UPROPERTY(EditAnywhere) 
	int ActiveTriggers = 1;
};
