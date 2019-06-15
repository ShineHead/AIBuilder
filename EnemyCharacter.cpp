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
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/DamageType.h"
#include "Public/DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/ArrowComponent.h"
#include "BTTAttackEnemy.h"
#include "Actors/Weapon.h"
#include "Sound/SoundBase.h"
#include "BTTaskNode_FallBack.h"
#include "Engine.h"
#include "AIModule/Classes/AIController.h"
#include "GameFramework/Controller.h"
#include "Components/PawnNoiseEmitterComponent.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(45.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(450.f);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	Arrow->SetupAttachment(GetRootComponent());
	
	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	Health = 120;
	bInCombat = false;
	bIsAttacking = false;
	inRange = false;
	SensedPawn = nullptr;
	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;
	DeathDelay = 3.f;
	DamageAmount = 22.0f;
	bisDead = false;
	bIsStunned = false;
	
	WeaponAttachSocketName = "weapon"; ///Attach Weapon to CharacterSocket
	
	

}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	EStatus = EAIMode::EMS_Patrol;
	bIsAttacking = false;
	ThisCharacterRef = this;


	///Set Teams, this effects pawn sensing
	ThisAICharacter = this;
	if (ThisAICharacter->ActorHasTag("Team1"))
	{
		SenseType = "Team2";
	}
	if (ThisAICharacter->ActorHasTag("Team2"))
	{
		SenseType = "Team1";
	}

	if (ThisAICharacter)
	{
		OriginalLocation = ThisAICharacter->GetActorLocation();
	}

	AIController = Cast<AEnemyAIController>(GetController()); ///reference to the AI Controller
	auto BehaviorTree = AIController->GetBlackboardComponent();
	AttackReference = Cast<UBTTAttackEnemy>(BehaviorTree);
	FallBackReference = Cast<UBTTaskNode_FallBack>(BehaviorTree);

	

	///Set Weapon on Character
	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		CurrentWeapon->SetOwner(this);
		ToggleDamage(false);
	
	}
	
	///call sense function
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePlayer); //onseepawn function gets the pawn
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnHearNoise); //onseepawn function gets the pawn

	}


}

void AEnemyCharacter::OnSeePlayer(APawn* Pawn) ///and sends it here
{
	if (AIController && ThisAICharacter != nullptr)
	{	
			if (Pawn->ActorHasTag(SenseType))
			{
				SensedPawn = Pawn;
				bInCombat = true;
				auto AILocation = ThisAICharacter->GetActorLocation();
				auto SensedPawnLocation = SensedPawn->GetActorLocation();
				EStatus = EAIMode::EMS_Hostile;
				//CheckDistance(AILocation, SensedPawnLocation);
				
			}	
			else 
			{
				EStatus = EAIMode::EMS_Patrol;
				SensedPawn = nullptr;
				

			}
	}
	
}


void AEnemyCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	
	if (AIController && PawnInstigator != this && PawnInstigator->ActorHasTag(SenseType))
	{
		SensedPawn = PawnInstigator;
		bInCombat = true;
		auto AILocation = ThisAICharacter->GetActorLocation();
		auto SensedPawnLocation = SensedPawn->GetActorLocation();
		EStatus = EAIMode::EMS_Hostile;
		//UE_LOG(LogTemp, Warning, TEXT("HEAR NOISE"));
	}

	else
	{
		EStatus = EAIMode::EMS_Patrol;
		
		
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

//void AEnemyCharacter::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//	
//}
//void AEnemyCharacter::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	
//}

void AEnemyCharacter::Attack()
{
	EStatus = EAIMode::EMS_Attacking;
	CurrentWeapon->WeaponHit(); //attack passed to AWeapon
	bIsAttacking = true;
		
		//if (SensedPawn)
		//{
		//	// Look toward focus
		//	FVector FocalPoint = ThisAICharacter->GetActorForwardVector();
		//	FVector Direction = FocalPoint - SensedPawn->GetActorLocation();
		//	FRotator NewControlRotation = Direction.Rotation();

		//	NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);
		//	ThisAICharacter->GetController()->SetControlRotation(NewControlRotation);
		//	
		//	//ThisAICharacter->SetActorRotation(NewControlRotation);
		//}
			

}
// Allow getting and Setting Health in Blueprint and from other Functions
float AEnemyCharacter::GetHealth()
{
	return Health;
}

//==========================================================================================
float AEnemyCharacter::SetHealth(float &HealthToSet)
{
	return Health;
}

//==========================================================================================

///ANIMATION NOTIFIES ---- this take a point in animation and calls the below functions

void AEnemyCharacter::HitTakenPlace() ///gets attack notify
{
	ToggleDamage(true);
}

void AEnemyCharacter::AttackingEnd()
{
	bIsAttacking = false;

	AttackReference->EndAttack();
	EStatus = EAIMode::EMS_Hostile;
	ToggleDamage(false);
}

//==========================================================================================


float AEnemyCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
	if (!bisDead)
	{
		Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		Health -= Damage;
		UE_LOG(LogTemp, Warning, TEXT("Health = %f"), Health)
		bIsStunned = true;
		bIsAttacking = false;
		EStatus = EAIMode::EMS_Stunned;
		SetHealthBar();

		if (Health <= 0)
		{
			bisDead = true;
			EStatus = EAIMode::EMS_Dying;
			Die();
			if (CurrentWeapon)
			{
				CurrentWeapon->SetLifeSpan(2.0f);
			}
		}
	}
	return Damage;
}

void AEnemyCharacter::FleeingEnd()
{
	


}

void AEnemyCharacter::StunnedEnd()
{
	bIsStunned = false;

	//TODO Check for sensed pawn, then choose hostile/patrol
	EStatus = EAIMode::EMS_Hostile;
	
}


void AEnemyCharacter::DeathAnimation_Implementation()
{



}


void AEnemyCharacter::Die()
{

	if (DeathSoundRef)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DeathSoundRef, 1, 0.8, 0.0, nullptr, this);
	}

	
	//GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetachFromControllerPendingDestroy();
	//GetMesh()->bPauseAnims = true;
	//GetMesh()->bNoSkeletonUpdate = true;
	SetLifeSpan(2.0f);
	CurrentWeapon->bDamageOn = false;
	DeathAnimation();

}

void AEnemyCharacter::DeathEnd()
{
	
}




void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}



void AEnemyCharacter::ReportNoise(USoundBase* SoundToPlay, float Volume, UParticleSystem* ShoeDust)
{
	
	if (SoundToPlay)
	{
		//Play the actual sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, GetActorLocation(), Volume);

		//Report that we've played a sound with a certain volume in a specific location
		MakeNoise(Volume, this, GetActorLocation());
	}
	if (ShoeDust)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShoeDust, GetActorLocation());
	}


}

void AEnemyCharacter::ToggleDamage(bool DamageOn)
{
	bDamageOn = DamageOn;
	CurrentWeapon->ToggleDamage(bDamageOn);
}

void AEnemyCharacter::SetHealthBar_Implementation()
{


}