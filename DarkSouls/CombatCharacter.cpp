// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"


// Sets default values
ACombatCharacter::ACombatCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetLocked = false;
	NextAttackReady = false;
	Attacking = false;
	AttackDamaging = false;
	MovingForward = true;
	MovingBackwards = false;
	RotateTowardsTarget = true;
	Stumbling = false;
	RotationSmoothing = 5.0f;
	LastRotationSpeed = 0.0f;
}

// Called when the game starts or when spawned
void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetMovingForward(true);

	
}

// Called every frame
void ACombatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotateTowardsTarget)
	{
		LookAtSmooth();
	}
}

// Called to bind functionality to input
void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACombatCharacter::Attack()
{
	Attacking = true;
	
	AttackDamaging = false;
	//SetMovingForward(false);

	AttackHitActors.Empty();
}

void ACombatCharacter::AttackLunge()
{
	if (Target != NULL)
	{
		//Locked Target
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		Direction = FVector(Direction.X,Direction.Y,0);
		FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		SetActorRotation(Rotation);
		
	}

	FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * 100);
	SetActorLocation(NewLocation,true);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DashParticle, GetActorLocation(), GetActorRotation());
}

void ACombatCharacter::EndAttack()
{
	Attacking = false;
	NextAttackReady = true;
	//SetMovingForward(true);
}

void ACombatCharacter::SetAttackDamaging(bool Damaging)
{
	AttackDamaging = Damaging;

}

void ACombatCharacter::SetMovingForward(bool IsMovingForward)
{
	MovingForward = IsMovingForward;
}

void ACombatCharacter::SetMovingBackwards(bool IsMovingBackwards)
{
	MovingBackwards = IsMovingBackwards;
}

void ACombatCharacter::EndStumble()
{
	Stumbling = false;
}

void ACombatCharacter::AttackNextReady()
{
	NextAttackReady = true;

	

}

void ACombatCharacter::LookAtSmooth()
{
	if (Target != NULL && TargetLocked && !Attacking && !GetCharacterMovement()->IsFalling())
	{ 
		//Target Smooth
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		Direction = FVector(Direction.X, Direction.Y, 0);

		//Real Direction
		FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		FRotator SmoothedRotation = FMath::Lerp(GetActorRotation(), Rotation, RotationSmoothing * GetWorld()->DeltaTimeSeconds);

		LastRotationSpeed = SmoothedRotation.Yaw - GetActorRotation().Yaw;

		SetActorRotation(SmoothedRotation);
	} 
}

float ACombatCharacter::GetCurrentRotationSpeed()
{
	if (RotateTowardsTarget)
	{
		return LastRotationSpeed;
	}

	return 0.0f;
}
