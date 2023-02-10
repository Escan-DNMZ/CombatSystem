// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SoulsCharacter.h"

AEnemyBase::AEnemyBase()
{
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(),"RightHandItem");
	Weapon->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	PrimaryActorTick.bCanEverTick = true;
	MovingBackwards = false;
	MovingForward = false;
	Interruptable = true;
	LastStumbleIndex = 0;



}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->HideBoneByName("weapon_r", EPhysBodyOp::PBO_None);

	ActiveState = State::IDLE;

	Target = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickStateMachine();
}

void AEnemyBase::TickStateMachine()
{
	switch (ActiveState)
	{
	case State::IDLE:
		StateIdle();
		break;

	case State::CHASE_CLOSE:
		StateChaseClose();
		break;

	case State::CHASE_FAR:
		StateChaseFar();
		break;

	case State::ATTACK:
		StateAttack();
		break;

	case State::STUMBLE:
		StateStumble();
		break;

	/*case State::TAUNT:
		StateTaunt();
		break;*/

	case State::DEAD:
		StateDead();
		break;

	default:
		break;
	}
}

void AEnemyBase::SetState(State NewState)
{
	if (ActiveState != State::DEAD)
	{
		ActiveState = NewState;
	}
}

void AEnemyBase::StateIdle()
{
	float Distance = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
	if (Target && Distance <= 1200.f) {
		TargetLocked = true;

		SetState(State::CHASE_CLOSE);
	}
}

void AEnemyBase::StateChaseClose()
{
	float Distance = FVector::Distance(Target->GetActorLocation(), GetActorLocation());

	if (Distance <= 20.0f)
	{
		FVector TargetDirection = Target->GetActorLocation() - GetActorLocation();
			Attack(true);
	}
	else
	{
		AAIController* AIController = Cast<AAIController>(Controller);

		AIController->MoveToActor(Target);
		
	}
}

void AEnemyBase::StateChaseFar()
{
}

void AEnemyBase::StateAttack()
{
	Attack(true);
}

void AEnemyBase::StateStumble()
{
}

void AEnemyBase::StateTaunt()
{
}

void AEnemyBase::StateDead()
{
}

void AEnemyBase::FocusTarget()
{
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void AEnemyBase::MoveForward()
{
}

void AEnemyBase::Attack(bool Rotate)
{
	if (Rotate)
	{

		for (UAnimMontage* Attack : Attacks)
		{
			PlayAnimMontage(Attack, 1.0f);

			Attacking = true;
			AttackLunge();
		}
		TSet<AActor*> WeaponOverlappingActor;
		for (auto& WeaponOverlaped : WeaponOverlappingActor)
		{

			ASoulsCharacter* Enemy = Cast<ASoulsCharacter>(WeaponOverlaped);
			if (Cast<ASoulsCharacter>(WeaponOverlaped)) {
				Enemy->AttackDamaging = true;
			}
		}
	}
	
}

void AEnemyBase::AttackNextReady()
{
}

void AEnemyBase::EndAttack()
{
}

void AEnemyBase::AttackLunge()
{
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

