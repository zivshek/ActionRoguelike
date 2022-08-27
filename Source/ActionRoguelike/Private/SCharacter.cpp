// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "SInteractionComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float Value)
{
	auto ControlRot = GetControlRotation();
	ControlRot.Pitch = 0;
	ControlRot.Roll = 0;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	auto ControlRot = GetControlRotation();
	ControlRot.Pitch = 0;
	ControlRot.Roll = 0;

    const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		SpawnProjectile(PrimaryProjectileClass);
	}), 0.2f, false);
}

void ASCharacter::SecondaryAttack()
{
	PlayAnimMontage(AttackAnim);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this]()
																								{
																									SpawnProjectile(SecondaryProjectileClass);
																								}), 0.2f, false);
}

void ASCharacter::TertiaryAttack()
{
	PlayAnimMontage(AttackAnim);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this]()
																								{
																									SpawnProjectile(SecondaryProjectileClass);
																								}), 0.2f, false);
}

void ASCharacter::PrimaryInteract()
{
	InteractionComp->PrimaryInteract();
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ProjectileClass)
{
	if (ensure(ProjectileClass))
	{
		const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		auto CamManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector Start = CamManager->GetCameraLocation();
		FVector End = Start + CamManager->GetCameraRotation().Vector() * AimRange;
		FHitResult Hit;
		const bool HasHit = GetWorld()->LineTraceSingleByProfile(Hit, Start, End, "Projectile");
		FVector Aim = HasHit ? Hit.ImpactPoint : End;

		const FTransform SpawnTM = FTransform((Aim - HandLocation).Rotation(), HandLocation);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

		{
			bool Persistent = false;
			float LifeTime = 2.0f;
			float Thickness = 2.0f;
			DrawDebugSphere(GetWorld(), Start, 10.0f, 16, FColor::Orange, Persistent, LifeTime, 0, Thickness);
			DrawDebugSphere(GetWorld(), End, 10.0f, 16, FColor::Yellow, Persistent, LifeTime, 0, Thickness);
			DrawDebugLine(GetWorld(), Start, Aim, FColor::Magenta, Persistent, LifeTime, 0, Thickness);
			DrawDebugLine(GetWorld(), HandLocation, Aim, FColor::Green, Persistent, LifeTime, 0, Thickness);
		}
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("TertiaryAttack", IE_Pressed, this, &ASCharacter::TertiaryAttack);
}

