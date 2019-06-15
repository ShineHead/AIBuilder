// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Classes/GameplayTagContainer.h"
#include "EnemyCharacter.generated.h"


class AWeapon;
class UPawnNoiseEmitterComponent;

UENUM(BlueprintType)
enum class EAIMode : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_Patrol			UMETA(DisplayName = "Patrol"),
	EMS_Hostile			UMETA(DisplayName = "Hostile"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),
	EMS_Stunned			UMETA(DisplayName = "Stunned"),
	EMS_Cheering		UMETA(DisplayName = "Cheering"),
	EMS_Fleeing			UMETA(DisplayName = "Fleeing"),
	EMS_Dying			UMETA(DisplayName = "Dying"),


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

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AWeapon>StarterWeaponClass;

	UPROPERTY()
	AWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USphereComponent* CombatSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	EAIMode EStatus;

	FORCEINLINE void SetEnemyMovementStatus(EAIMode EnemyMovementStatus) { EStatus = EnemyMovementStatus; }

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnSense)
	float SightDistance;
		
	UPROPERTY()
	bool inRange;

	UPROPERTY()
	FVector OriginalLocation;

	UPROPERTY()
	APawn* SensedPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnSense)
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage)
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damage)
	class AItem* ActiveOverlappingItem;

	UPROPERTY()
	class UBTTAttackEnemy* AttackReference;

	UPROPERTY()
	class UBTTaskNode_FallBack* FallBackReference;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	bool bInCombat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	bool bIsStunned;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	bool bIsFleeing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class USoundBase* DeathSoundRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float CharacterDamageAmount;


	UPROPERTY()
	AEnemyCharacter* ThisCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float Health;



protected:
	

	
	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	UFUNCTION(BlueprintCallable, Category = AI)
	void ReportNoise(USoundBase* SoundToPlay, float Volume, class UParticleSystem* ShoeDust);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPawnNoiseEmitterComponent* PawnNoiseEmitterComp;

	

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

	UFUNCTION(Blueprintpure, Category = AI)
	float SetHealth(float &HealthToSet);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void SetHealthBar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
	void DeathAnimation();

	/*UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/
	

	//UFUNCTION()
	//static float ApplyDamage(AActor * DamagedActor, float BaseDamage, AController * EventInstigator, AActor * DamageCauser, TSubclassOf < class UDamageType > DamageTypeClass);

	UFUNCTION()
	void Attack();


	UFUNCTION(BlueprintCallable, Category = "AI")
	void AttackingEnd();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void FleeingEnd();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StunnedEnd();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void HitTakenPlace();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void Die();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void DeathEnd();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void ToggleDamage(bool bisdamage);

	UPROPERTY()
	bool bDamageOn;


	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bVictoy;
};
