// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingCharacter.h"
#include "ShootingProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AShootingCharacter

AShootingCharacter::AShootingCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//// Create a gun mesh component
	//PrimaryWeapon  = 
	//FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	//FP_Gun->bCastDynamicShadow = false;
	//FP_Gun->CastShadow = false;
	//// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	//FP_Gun->SetupAttachment(RootComponent);

	//FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	//FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(78.2f, -2.0f, 9.0f));

	// Default offset from the character location for projectiles to spawn
	//GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	//// Create VR Controllers.
	//R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	//R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	//R_MotionController->SetupAttachment(RootComponent);
	//L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	//L_MotionController->SetupAttachment(RootComponent);

	//// Create a gun and attach it to the right-hand VR controller.
	//// Create a gun mesh component
	//VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	//VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	//VR_Gun->bCastDynamicShadow = false;
	//VR_Gun->CastShadow = false;
	//VR_Gun->SetupAttachment(R_MotionController);
	//VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	//VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	//VR_MuzzleLocation->SetupAttachment(VR_Gun);
	//VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	//VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AShootingCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	if (WeaponClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FVector Localtion = FVector(0.f, 0.f, 0.f);
			FRotator Rotator = FRotator(0.f);
			PrimaryWeapon = World->SpawnActor<AWeaponKnife>(WeaponClass, Localtion, Rotator);
			PrimaryWeapon->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot"));
			CurrentWeapon = EWeapon::EW_Knife;
		}
	}
	
	Mesh1P->SetHiddenInGame(false, true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShootingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind Weapon Change Event
	PlayerInputComponent->BindAction("WeaponChange1", IE_1, this, &AShootingCharacter::OnHoldRifle);
	PlayerInputComponent->BindAction("WeaponChange2", IE_2, this, &AShootingCharacter::OnHoldPisto);


	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShootingCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AShootingCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AShootingCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShootingCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShootingCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShootingCharacter::LookUpAtRate);
}

void AShootingCharacter::OnHoldRifle()
{

}


void AShootingCharacter::OnHoldPisto()
{

}

void AShootingCharacter::OnFire()
{
	if(CurrentWeapon == EWeapon::EW_Knife)
	{
		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		// auto assetMontage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Knife_Attack_01_anim_Montage.Arms_Knife_Attack_01_anim_Montage'"));
		//const ConstructorHelpers::FObjectFinder<UAnimMontage> Combo1Finder = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Knife_Attack_01_anim_Montage.Arms_Knife_Attack_01_anim_Montage'"));
		//if (Combo1Finder.Succeeded())
		//{
		//	FireAnimation = Combo1Finder.Object;
		//}

		// Load static asset
		FString knifeAttackMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Knife_Attack_01_anim_Montage.Arms_Knife_Attack_01_anim_Montage'"));
		UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *knifeAttackMontage));
		FireAnimation = assetMontage;


		//FireAnimation = assetMontage.Object;
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
	else
	{
		// try and fire a projectile
		//if (ProjectileClass != nullptr)
		//{
		//	UWorld* const World = GetWorld();
		//	if (World != nullptr)
		//	{
		//		if (bUsingMotionControllers)
		//		{
		//			const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
		//			const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
		//			World->SpawnActor<AShootingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		//		}
		//		else
		//		{
		//			const FRotator SpawnRotation = GetControlRotation();
		//			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		//			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		//			//Set Spawn Collision Handling Override
		//			FActorSpawnParameters ActorSpawnParams;
		//			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		//			// spawn the projectile at the muzzle
		//			World->SpawnActor<AShootingProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		//		}
		//	}
		//}
	}
}

//void AShootingCharacter::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}

void AShootingCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		//OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AShootingCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

void AShootingCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AShootingCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AShootingCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AShootingCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShootingCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AShootingCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AShootingCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AShootingCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AShootingCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}
