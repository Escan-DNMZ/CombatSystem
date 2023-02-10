// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsCharacter.h"

#include "GameFramework/Actor.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/CameraShake.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyBase.h"
#include "Engine/SkeletalMesh.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASoulsCharacter::ASoulsCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstAttack = false;

	TargetLockDistance = 800.0f;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);

	// The camera follows at this distance behind the character
	CameraBoom->TargetArmLength = 500.0f;

	// Rotate the arm based on the controller
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	// Attach the camera to the end of the boom and let the boom adjust 
	// to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Camera does not rotate relative to arm
	FollowCamera->bUsePawnControlRotation = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), "RightHandItem");
	
	
	Weapon->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	EnemyDetectionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Enemy Detection Collider"));
	EnemyDetectionCollider->SetupAttachment(RootComponent);
	EnemyDetectionCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	EnemyDetectionCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,
		ECollisionResponse::ECR_Overlap);
	EnemyDetectionCollider->SetSphereRadius(TargetLockDistance);

	Attacking = false;
	Rolling = false;
	TargetLocked = false;
	NextAttackReady = false;
	AttackDamaging = false;
	AttackIndex = 0;

	PassiveMovementSpeed = 450.0f;
	CombatMovementSpeed = 250.0f;
	GetCharacterMovement()->MaxWalkSpeed = PassiveMovementSpeed;

	
}

// Called when the game starts or when spawned
void ASoulsCharacter::BeginPlay()
{

	Super::BeginPlay();

	FirstAttack = true;

	

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

	EnemyDetectionCollider->OnComponentBeginOverlap.
		AddDynamic(this, &ASoulsCharacter::OnEnemyDetectionBeginOverlap);
	EnemyDetectionCollider->OnComponentEndOverlap.
		AddDynamic(this, &ASoulsCharacter::OnEnemyDetectionEndOverlap);

	TSet<AActor*> NearActors;
	EnemyDetectionCollider->GetOverlappingActors(NearActors);
	

	for (auto& EnemyActors :NearActors)
	{
		if (Cast<AEnemyBase>(EnemyActors))
		{
			NearbyEnemies.Add(EnemyActors);
			
		}
	}
}

// Called every frame
void ASoulsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FocusTarget();

	if (Rolling)
	{
		AddMovementInput(GetActorForwardVector(), 600 * GetWorld()->DeltaTimeSeconds);
	}
	else if (Stumbling && MovingBackwards) {
		AddMovementInput(-GetActorForwardVector(), 40.0f * GetWorld()->DeltaTimeSeconds);
	}
	else if (Attacking && AttackDamaging)
	{
	
		Weapon->GetOverlappingActors(WeaponOverlappingActor);
		for (auto& WeaponOverlaped:WeaponOverlappingActor)
		{
			if (WeaponOverlaped == this)
			{
				continue;
			}

			//Eðer WeaponOverlaped edilen aktör herhangi bir hasar almadýysa bu if'e girer
			if (!AttackHitActors.Contains(WeaponOverlaped))
			{
				//Actor hasar alýr
				float AppliedDamage = 0.0f; //Health Component'ý yaptýkdan sonra

				if (AppliedDamage > 0.0f)
				{
					//Ve Actor hasar alýnan actorler dizinine eklenir
					AttackHitActors.Add(WeaponOverlaped);

					GetWorld()->GetFirstPlayerController()->
						PlayerCameraManager->StartCameraShake(CameraShakeMinor);

					
				}
			}
		}

	}
	if (Target != NULL && TargetLocked)
	{
		FVector TargetDirection = Target->GetActorLocation() - GetActorLocation();
	
		//x ve ya kordinatlarý arasýndaki uzunluðu alýyor
		if (TargetDirection.Size2D() > 400) {
			FRotator Differance = UKismetMathLibrary::NormalizedDeltaRotator(Controller->GetControlRotation(),TargetDirection.ToOrientationRotator());
			
			if (FMath::Abs(Differance.Yaw) > 30.0f)
			{
				AddControllerYawInput(DeltaTime * -Differance.Yaw * 0.5f);
			}
		}
	}
}

// Called to bind functionality to input
void ASoulsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASoulsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASoulsCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("CombatModeToggle", IE_Pressed, this,
		&ASoulsCharacter::ToggleCombatMode);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this,
		&ASoulsCharacter::Attack);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this,
		&ASoulsCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this,
		&ASoulsCharacter::UnSprint);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this,
		&ASoulsCharacter::Roll);
	PlayerInputComponent->BindAction("CycleTarget+", IE_Pressed, this,
		&ASoulsCharacter::CycleTargetClockwise);
	PlayerInputComponent->BindAction("CycleTarget-", IE_Pressed, this,
		&ASoulsCharacter::CycleTargetCounterClockwise);
}

void ASoulsCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}
void ASoulsCharacter::UnSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = 450.f;
}

void ASoulsCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASoulsCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASoulsCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !Attacking && !Rolling && !Stumbling && MovingForward)
	{
		FRotator Rotation = GetControlRotation();
		FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
	InputDirection.X = Value;
}

void ASoulsCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !Attacking && !Rolling && !Stumbling)
	{
		FRotator Rotation = GetControlRotation();
		FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
	InputDirection.Y = Value;
}

void ASoulsCharacter::CycleTarget(bool Clockwise)
{
}

void ASoulsCharacter::CycleTargetClockwise()
{
}

void ASoulsCharacter::CycleTargetCounterClockwise()
{
}

void ASoulsCharacter::LookAtSmooth()
{
}



float ASoulsCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void ASoulsCharacter::OnEnemyDetectionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	Target = Enemy;

	TargetLocked = true;
}

void ASoulsCharacter::OnEnemyDetectionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Target = NULL;

	TargetLocked = false;
}

void ASoulsCharacter::Attack()
{
	Super::Attack();

	FTransform effect = Weapon->GetSocketTransform("effect");
	if (FirstAttack) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponEffect, effect);
		PlayAnimMontage(Attacks[0], 1.0f);
		AttackLunge();
		FirstAttack = false;
		GetWorld()->GetFirstPlayerController()->
			PlayerCameraManager->StartCameraShake(CameraShakeMinor);
	}

	if (NextAttackReady)
	{
		int s = FMath::RandRange(0, Attacks.Num() - 1);
		int AttackParticle1 = FMath::RandRange(0, 2);
		

		AttackLunge();

		PlayAnimMontage(Attacks[s], 1.0f);

		if (AttackParticle1 == 1)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponEffect2, effect, true);
		}
		else if (AttackParticle1 == 2) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponEffect3, effect, true);
		}
		
		NextAttackReady = false;

		GetWorld()->GetFirstPlayerController()->
			PlayerCameraManager->StartCameraShake(CameraShakeMinor);

	}
	
}

void ASoulsCharacter::EndAttack()
{
	Super::EndAttack();
}

void ASoulsCharacter::Roll()
{
}

void ASoulsCharacter::StartRoll()
{
}

void ASoulsCharacter::EndRoll()
{
}

void ASoulsCharacter::RollRotateSmooth()
{
}

void ASoulsCharacter::FocusTarget()
{
}

void ASoulsCharacter::ToggleCombatMode()
{
}

void ASoulsCharacter::SetInCombat(bool InCombat)
{
}
