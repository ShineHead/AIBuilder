// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIControllerCPP.generated.h"

class APawn;

UCLASS()
class TIBERIANAWAKENING_API AAIControllerCPP : public AAIController
{
	GENERATED_BODY()

public:
	AAIControllerCPP();

	virtual void BeginPlay() override;

	virtual void Possess(APawn* Pawn) override;

	virtual void Tick(float DeltaSeconds) override;

public:

	FBlackboard::FKey TargetKeyID;

	FBlackboard::FKey AIStateBlackboardkey;
	//AIComponent References
	UBehaviorTreeComponent* BehaviorComp;
	UBlackboardComponent* BlackboardComp;

	





	
};
