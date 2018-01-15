// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"


AMovingPlatform::AMovingPlatform()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	MoveDirection = (GetTransform().TransformPosition(TargetLocation) - GetActorLocation()).GetSafeNormal();
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
	JourneyLength = (GlobalTargetLocation- GlobalStartLocation).Size();
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && ActiveTriggers >= 1)
	{
		FVector Location = GetActorLocation();
		float JourneyTraveled = (Location - GlobalStartLocation).Size();

		if (JourneyTraveled < JourneyTolerance)
		{
			MoveDirection = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
		}
		if (JourneyTraveled >= JourneyLength)
		{
			MoveDirection = (GlobalStartLocation - GlobalTargetLocation).GetSafeNormal();
		}
		Location += (Speed * DeltaTime) * MoveDirection;
		SetActorLocation(Location);
	}

}

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	ActiveTriggers--;
}
