// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "OrbirintMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4COOKBOOK_API UOrbirintMovementComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrbirintMovementComponent();

	UPROPERTY()
	bool RotateToFaceOutwards;

	UPROPERTY()
	float RotationSpeed;

	UPROPERTY()
	float OrbitDistance;

	UPROPERTY()
	float CurrentValue;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
