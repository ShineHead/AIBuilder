// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControllerCPP.h"
#include "CharacterCPP.h"
#include "Waypoint.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "TiberianAwakeningCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"


AAIControllerCPP::AAIControllerCPP()
{
	PrimaryActorTick.bCanEverTick = true;


	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));


}

void AAIControllerCPP::BeginPlay()
{
	Super::BeginPlay();
	


}

void AAIControllerCPP::Possess(APawn* Pawn)
{
	Super::Possess(Pawn);

	auto Character = Cast<ACharacterCPP>(Pawn);




	///Handles, Start Tree
	if (Character && Character->BehaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*Character->BehaviorTree->BlackboardAsset); /// initialise blackboard
		TargetKeyID = BlackboardComp->GetKeyID("ActorTarget"); ///get the blackboard variable
		BehaviorComp->StartTree(*Character->BehaviorTree); ///start the tree, referencing the character's BT

																//initialise Enum
		AIStateBlackboardkey = BlackboardComp->GetKeyID("AIState");
	}


}


void AAIControllerCPP::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}


