// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Classes/GameplayTagContainer.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),

	EMS_MAX				UMETA(DisplayName = "DefaultMax")
};



UCLASS()
class UE4COOKBOOK_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USphereComponent* EnemySphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) {EnemyMovementStatus = Status; }

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* EnemyBehaviorTree;

	/*The Component which is used for the "seeing" sense of the AI*/
	UPROPERTY(EditAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY()
	AEnemyCharacter* ThisAICharacter;

	UPROPERTY()
	AEnemyCharacter* EnemyCharacter;

	UPROPERTY()
	int Spawncount;

	UPROPERTY(EditAnywhere)
	int Team;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnSen)
	float SightDistance;
		
	UPROPERTY()
	bool inRange;

	UPROPERTY()
	APawn* SensedPawn;

	UPROPERTY()
	FName SenseType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	class AEnemyAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking)
	bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking)
	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking)
	float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attacking)
	float AttackMaxTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Anims)
	class UAnimMontage* CombatMontage;

	UPROPERTY()
	bool bOverlappingCombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage)
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	class AWeaponBase* CurrentWeapon;

	UPROPERTY()
	class UBTTAttackEnemy* AttackReference;

	UPROPERTY()
	const USkeletalMeshSocket* RightHandSocket;
	
	UPROPERTY()
	const USkeletalMeshComponent* ThisMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damage)
	UArrowComponent* Arrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	bool bisDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
		FTimerHandle DeathTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	float DeathDelay;

private:
	
	UPROPERTY()
	float Health;
	
	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	float CheckDistance(FVector& PointA, FVector& PointB);

	UFUNCTION()
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Blueprintpure, Category = AI)
	float GetHealth();

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

	//UFUNCTION()
	//static float ApplyDamage(AActor * DamagedActor, float BaseDamage, AController * EventInstigator, AActor * DamageCauser, TSubclassOf < class UDamageType > DamageTypeClass);

	UFUNCTION()
	void Attack();


	UFUNCTION(BlueprintCallable, Category = "Animation")
	void AttackingEnd();


	UFUNCTION(BlueprintCallable, Category = "Animation")
	void HitTakenPlace();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void Die();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void DeathEnd();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void Disappear();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void StopMovement();
};
