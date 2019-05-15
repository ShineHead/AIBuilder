// Fill out your copyright notice in the Description page of Project Settings.

#include "OrbirintMovementComponent.h"

// Sets default values for this component's properties
UOrbirintMovementComponent::UOrbirintMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	RotationSpeed = 10;
	OrbitDistance = 100;
	CurrentValue = 0;
	RotateToFaceOutwards = true;

}


// Called when the game starts
void UOrbirintMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOrbirintMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	float CurrentValueInRadians = FMath::DegreesToRadians<float>(CurrentValue);

	SetRelativeLocation(FVector(OrbitDistance * FMath::Cos(CurrentValueInRadians), OrbitDistance * FMath::Sin(CurrentValueInRadians), RelativeLocation.Z));


	if (RotateToFaceOutwards)
	{
		FVector LookDir = (RelativeLocation).GetSafeNormal();
		FRotator LookAtRot = LookDir.Rotation();
		SetRelativeRotation(LookAtRot);
	}
	CurrentValue = FMath::Fmod(CurrentValue + (RotationSpeed * DeltaTime), 360);

}

