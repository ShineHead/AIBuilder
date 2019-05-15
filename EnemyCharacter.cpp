// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SphereComponent.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"
#include "AI/EnemyAIController.h"
#include "GameFramework/Pawn.h"
#include "Public/TimerManager.h"
#include "Actors/Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/DamageType.h"
#include "Public/DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/ArrowComponent.h"
#include "BTTAttackEnemy.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Health = 100.0f;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(45.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(450.f);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	Arrow->SetupAttachment(GetRootComponent());

	bOverlappingCombatSphere = false;

	bIsAttacking = false;
	bool inRange = false;
	SensedPawn = nullptr;

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;
	DeathDelay = 3.f;

	Health = 100;
	DamageAmount = 50;

	bisDead = false;
}



void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	ThisAICharacter = this;
	AIController = Cast<AEnemyAIController>(GetController()); ///reference to the AI Controller
	auto BehaviorTree = AIController->GetBlackboardComponent();
	AttackReference = Cast<UBTTAttackEnemy>(BehaviorTree);


	//CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyCharacter::CombatSphereOnOverlapBegin);
	//CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyCharacter::CombatSphereOnOverlapEnd);

	if (ThisAICharacter->ActorHasTag("Team1"))
	{
		
		SenseType = "Team2";
	}
	if (ThisAICharacter->ActorHasTag("Team2"))
	{
		
		SenseType = "Team1";
	}	

	//RightHandSocket = ThisAICharacter->GetMesh()->GetSocketByName("Bip001-R-Hand");
	//auto ThisMesh = ThisAICharacter->GetMesh();
	


	///call sense function
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePlayer); //onseepawn function gets the pawn
	}
}

void AEnemyCharacter::OnSeePlayer(APawn* Pawn) ///and sends it here
{
	if (AIController && ThisAICharacter != nullptr && bisDead == false)
	{	
			if (Pawn->ActorHasTag(SenseType))
			{
				SensedPawn = Pawn;
				auto AILocation = ThisAICharacter->GetActorLocation();
				auto SensedPawnLocation = SensedPawn->GetActorLocation();
				//CheckDistance(AILocation, SensedPawnLocation);
				//UE_LOG(LogTemp, Warning, TEXT("Player"));
			}	
	}
}
float AEnemyCharacter::CheckDistance(FVector& PointA, FVector& PointB)
{
	float Distance = (PointA - PointB).Size();
	SightDistance = Distance;
	UE_LOG(LogTemp, Warning, TEXT("SightDistance%f"), SightDistance);
	if (SightDistance < 2000.0)
	{
		inRange = true;
	}
	else
	{
		inRange = false;
	}
	return Distance;
}

void AEnemyCharacter::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
}
void AEnemyCharacter::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemyCharacter::Attack()
{
		//UE_LOG(LogTemp, Warning, TEXT("Attacking!"));
		bIsAttacking = true;

		//FDamageEvent DamageEvent(UDamageType::StaticClass());
		//SensedPawn->TakeDamage(DamageAmount, DamageEvent, AIController, this);
}

//float AEnemyCharacter::ApplyDamage(AActor * DamagedActor, float BaseDamage, AController * EventInstigator, AActor * DamageCauser, TSubclassOf < class UDamageType > DamageTypeClass)
//{
//
//}





float AEnemyCharacter::GetHealth()
{
	return Health;
}


///after attack notify hits with sword, this is executed
void AEnemyCharacter::HitTakenPlace()
{
	FHitResult HitResult;
	auto StartLocation = Arrow->GetComponentLocation();
	FRotator ArrowRotation = Arrow->GetComponentRotation();

	FVector EndLocation = StartLocation + ArrowRotation.Vector() * 200.0f;

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(0, 0, 255), false, 2.f, 0.f, 5.f);

	/// Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		StartLocation,
		EndLocation,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		TraceParameters
	);
	
	AActor* PawnHit = HitResult.GetActor();
	if(PawnHit)
	{
		if (PawnHit->ActorHasTag(SenseType))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit %s !!!!!"), *(PawnHit->GetName()));
			FDamageEvent DamageEvent(UDamageType::StaticClass());
			PawnHit->TakeDamage(DamageAmount, DamageEvent, AIController, this);
		}
	}

}

float AEnemyCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bisDead)
	{
		Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		Health -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Health = %f"), Health)

			if (Health <= 0)
			{
				Die();
			}
	}
	

	return Damage;
}


void AEnemyCharacter::Die()
{
	bIsAttacking = false;
	bisDead = true;
	inRange = false;
	
}

void AEnemyCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	StopMovement();
	

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyCharacter::Disappear, DeathDelay);
}

void AEnemyCharacter::Disappear()
{
	Destroy();
}



void AEnemyCharacter::AttackingEnd()
{
	bIsAttacking = false;
	
	AttackReference->EndAttack();
	//GetCapsuleComponent()->SetCollionEnabled(ECollisionEnabled::NoCollision);



	//UE_LOG(LogTemp, Warning, TEXT("EndOfAttack"));

}







//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//if (AnimInstance && CombatMontage)
//{
//	AnimInstance->Montage_Play(CombatMontage, 1.35f);
//	AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
//
//}
//{
//
//	float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
//	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::Attack, AttackTime);
//}*/

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AEnemyCharacter::StopMovement()
{


}