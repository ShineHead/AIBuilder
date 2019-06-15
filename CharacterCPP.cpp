// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterCPP.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TiberianAwakening/AIControllerCPP.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Public/NavigationSystem.h"
#include "BehaviorTree/BTTAIAttack.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Public/TimerManager.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/DamageType.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Actors/ProjectileBase.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ACharacterCPP::ACharacterCPP()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 160.0f, 0.0f);

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(45.f);

	RightHandSocket = this->GetMesh()->GetSocketByName("hand_r");

	Rifle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Rifle"));
	Rifle->SetupAttachment(GetMesh());


	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	Arrow->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Arrow->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	Radius = 1500.f;
	bisDead = false;

	Health = 100.f;
}

// Called when the game starts or when spawned
void ACharacterCPP::BeginPlay()
{
	Super::BeginPlay();

	ThisAICharacter = this;
	AIController = Cast<AAIControllerCPP>(GetController()); ///reference to the AI Controller
	auto BehaviorTree = AIController->GetBlackboardComponent();
	AttackReference = Cast<UBTTAIAttack>(BehaviorTree);


	

	if (ThisAICharacter->ActorHasTag("GDI"))
	{

		EnemyTag = "NOD";
	}
	if (ThisAICharacter->ActorHasTag("NOD"))
	{

		EnemyTag = "GDI";
	}

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ACharacterCPP::OnSeePlayer); //onseepawn function gets the pawn
	}
	
}

// Called every frame
void ACharacterCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterCPP::GetSelectedAI(FVector MousePosition)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	
	if (NavSys)
	{
		FNavLocation Result;
		FVector Origin = MousePosition;

		UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *Origin.ToString());

		bool bSuccess = NavSys->GetRandomReachablePointInRadius(Origin, Radius, Result);
		if (bSuccess)
		{
			FVector Location = Result;
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(ThisAICharacter->GetController(), Location);
		}
	}
	
}

void ACharacterCPP::IsSelected(bool IsSelected)
{
	bIsCurrentlySelected = IsSelected;
	//UE_LOG(LogTemp, Warning, TEXT("AI Selected: %s"), bIsCurrentlySelected ? TEXT("True") : TEXT("false"));
}


void ACharacterCPP::OnSeePlayer(APawn* Pawn) ///and sends it here
{
	if (AIController && ThisAICharacter != nullptr && bisDead == false)
	{
		if (Pawn->ActorHasTag(EnemyTag))
		{
			SensedPawn = Pawn;
			auto AILocation = ThisAICharacter->GetActorLocation();
			auto SensedPawnLocation = SensedPawn->GetActorLocation();
			//CheckDistance(AILocation, SensedPawnLocation);
			
			
		}
	}
}

void ACharacterCPP::Attack()
{
	bIsAttacking = true;
	
}

void ACharacterCPP::NotifyFire()
{
	OnFire();
	UE_LOG(LogTemp, Warning, TEXT("Attacking"))
}

void ACharacterCPP::NotifyEndOfFire()
{
	
	bIsAttacking = false;

	AttackReference->EndAttack();
}

void ACharacterCPP::OnFire()
{
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{

			const FRotator SpawnRotation = ThisAICharacter->GetMesh()->GetComponentRotation() + FRotator (0.f, 90.0, 0.f);
			const FVector SpawnLocation = ThisAICharacter->GetMesh()->GetComponentLocation() + FVector(0.0, 0.f, 100.f);;
			
			ThisProjectile = World->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}
}

float ACharacterCPP::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bisDead)
	{
		Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		Health -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Health = %f"), Health)

			if (Health <= 0)
			{
			}
	}


	return Damage;
}
