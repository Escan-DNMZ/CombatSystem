// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatCharacter.h"
#include "SoulsCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DARKSOULS_API ASoulsCharacter : public ACombatCharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ASoulsCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Sprint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool FirstAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class UParticleSystem* WeaponEffect;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class UParticleSystem* WeaponEffect2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class UParticleSystem* WeaponEffect3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		class UParticleSystemComponent* WeaponPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class USkeletalMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class USphereComponent* EnemyDetectionCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float PassiveMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		float CombatMovementSpeed;

	void CycleTarget(bool Clockwise = true);

	UFUNCTION()
		void CycleTargetClockwise();

	UFUNCTION()
		void CycleTargetCounterClockwise();

	void LookAtSmooth();

	void UnSprint();

	TSet<AActor*> WeaponOverlappingActor;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, Category = "Animations")
		TArray<class UAnimMontage*> Attacks;

	UPROPERTY(EditAnywhere, Category = "Animations")
		class UAnimMontage* CombatRoll;

	UPROPERTY(EditAnywhere, Category = Camera)
		TSubclassOf<class UMatineeCameraShake> CameraShakeMinor;


	bool Rolling;
	FRotator RollRotation;

	int AttackIndex;
	float TargetLockDistance;

	TArray<class AActor*> NearbyEnemies;
	int LastStumbleIndex;

	FVector InputDirection;

	UFUNCTION()
		void OnEnemyDetectionBeginOverlap(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEnemyDetectionEndOverlap(class UPrimitiveComponent*
			OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

protected:

	void MoveForward(float Value);
	void MoveRight(float Value);

	void Attack();
	void EndAttack();

	void Roll();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void StartRoll();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		void EndRoll();

	void RollRotateSmooth();
	void FocusTarget();
	void ToggleCombatMode();
	void SetInCombat(bool InCombat);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:

	class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}

	class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	class USkeletalMeshComponent* GetWeapon() const
	{
		return Weapon;
	}

};
