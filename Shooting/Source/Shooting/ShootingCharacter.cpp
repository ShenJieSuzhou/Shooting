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
	
	// Load Weapon
	SetWeapons();
	doOnce = false;
	WeaponChanged = false;
	IsReloading = false;
	WeaponRifle->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Palm_R"));
	WeaponPisto->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Palm_R"));
	WeaponKnife->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Palm_R"));
	
	CurrentWeapon = WeaponRifle;
	CurWeaponType = EWeapon::EW_AK;
	WeaponType = 0;
	IsAimDown = false;

	Mesh1P->SetHiddenInGame(false, true);
	WeaponRifle->FP_Gun->SetHiddenInGame(false);
	WeaponPisto->FP_Gun->SetHiddenInGame(true);
	WeaponKnife->FP_Gun->SetHiddenInGame(true);

	hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}

void AShootingCharacter::SetWeapons()
{
	//Blueprint
	//UClass* WeaponKnifeClass = LoadClass<AWeaponBase>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/knife_BP.knife_BP_C'"));
	//UClass* WeaponRifleClass = LoadClass<AWeaponBase>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/AK_BP.AK_BP_C'"));
	//UClass* WeaponPistoClass = LoadClass<AWeaponBase>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/Glock_BP.Glock_BP_C'"));

	UClass* WeaponKnifeClass = LoadClass<AWeaponBase>(nullptr, TEXT("Class'/Script/Shooting.WeaponKnife'"));
	UClass* WeaponRifleClass = LoadClass<AWeaponBase>(nullptr, TEXT("Class'/Script/Shooting.WeaponAK'"));
	UClass* WeaponPistoClass = LoadClass<AWeaponBase>(nullptr, TEXT("Class'/Script/Shooting.WeaponGlock'"));

	UWorld* const World = GetWorld();
	FVector Localtion = FVector(0.f, 0.f, 0.f);
	FRotator Rotator = FRotator(0.f);
	
	if(WeaponKnifeClass != nullptr && WeaponRifleClass != nullptr && WeaponPistoClass != nullptr)
	{
		if(World != nullptr)
		{
			WeaponKnife = Cast<AWeaponKnife>(World->SpawnActor<AWeaponBase>(WeaponKnifeClass, FVector(8.9f, 2.0f, -2.7f), Rotator));
			WeaponPisto = Cast<AWeaponGlock>(World->SpawnActor<AWeaponBase>(WeaponPistoClass, Localtion, Rotator));
			WeaponRifle = Cast<AWeaponAK>(World->SpawnActor<AWeaponBase>(WeaponRifleClass, Localtion, Rotator));
		}
	}
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
	PlayerInputComponent->BindAction("HoldRifle", IE_Pressed, this, &AShootingCharacter::OnHoldRifle);
	PlayerInputComponent->BindAction("HoldPisto", IE_Pressed, this, &AShootingCharacter::OnHoldPisto);
	PlayerInputComponent->BindAction("HoldKnife", IE_Pressed, this, &AShootingCharacter::OnHoldKnife);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShootingCharacter::OnReload);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShootingCharacter::OnFire);

	//// Bind aim down
	//PlayerInputComponent->BindAction("AimDown", IE_Pressed, this, &AShootingCharacter::OnAimDownSight);
	//PlayerInputComponent->BindAction("AimDown", IE_Released, this, &AShootingCharacter::OnRecoverAimDownSight);

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
	if(!IsReloading)
	{
		CurrentWeapon = WeaponRifle;

		WeaponRifle->FP_Gun->SetHiddenInGame(false);
		WeaponPisto->FP_Gun->SetHiddenInGame(true);
		WeaponKnife->FP_Gun->SetHiddenInGame(true);
		CurWeaponType = EWeapon::EW_AK;
		WeaponType = 0;
		hud->UpdateAmmo(CurrentWeapon->AmmoCount, CurrentWeapon->MagazineAmmo, CurrentWeapon->MaxAmmoCount);
	}
}

void AShootingCharacter::OnHoldPisto()
{
	if (!IsReloading)
	{
		CurrentWeapon = WeaponPisto;

		WeaponKnife->FP_Gun->SetHiddenInGame(true);
		WeaponRifle->FP_Gun->SetHiddenInGame(true);
		WeaponPisto->FP_Gun->SetHiddenInGame(false);
		CurWeaponType = EWeapon::EW_Pisto;
		WeaponType = 1;
		hud->UpdateAmmo(CurrentWeapon->AmmoCount, CurrentWeapon->MagazineAmmo, CurrentWeapon->MaxAmmoCount);
	}
}

void AShootingCharacter::OnHoldKnife()
{
	if (!IsReloading)
	{
		WeaponChanged = false;
		CurrentWeapon = WeaponKnife;

		WeaponRifle->FP_Gun->SetHiddenInGame(true);
		WeaponPisto->FP_Gun->SetHiddenInGame(true);
		WeaponKnife->FP_Gun->SetHiddenInGame(false);
		CurWeaponType = EWeapon::EW_Knife;
		WeaponType = 2;
		hud->UpdateAmmo(0, 0, 0);
	}
}

void AShootingCharacter::OnFire()
{
	if(CurWeaponType == EWeapon::EW_Knife)
	{
		WeaponKnife->OnFire(Mesh1P);
	}
	else if(CurWeaponType == EWeapon::EW_AK)
	{
		if(!IsReloading)
		{
			if (WeaponRifle->OnCheckAmmo())
			{
				WeaponRifle->OnFire(Mesh1P, IsAimDown);
			}
		}
	}
	else if(CurWeaponType == EWeapon::EW_Pisto)
	{
		if (!IsReloading)
		{
			if (WeaponPisto->OnCheckAmmo())
			{
				WeaponPisto->OnFire(Mesh1P, IsAimDown);
			}
		}	
	}
}

void AShootingCharacter::OnReload()
{
	if (CurWeaponType == EWeapon::EW_Knife)
	{
		
	}
	else if (CurWeaponType == EWeapon::EW_AK)
	{
		if(WeaponRifle->MaxAmmoCount != 0)
		{
			if(!doOnce)
			{
				IsReloading = true;
				doOnce = true;

				WeaponRifle->OnReload(Mesh1P);
				// Load static asset
				FString ArmsAKReloadMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_AK_Reload_anim_Montage.Arms_AK_Reload_anim_Montage'"));
				UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *ArmsAKReloadMontage));
				if (assetMontage != nullptr)
				{
					// Get the animation object for the arms mesh
					UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
					if (AnimInstance != nullptr)
					{
						AnimInstance->Montage_Play(assetMontage, 1.f);
					}
				}

				IsReloading = false;
				doOnce = false;
			}
		}
		else
		{
			// ��ʾ��ҩ����
		}
	}
	else if (CurWeaponType == EWeapon::EW_Pisto)
	{
		if(WeaponPisto->MaxAmmoCount != 0)
		{
			if (!doOnce)
			{
				IsReloading = true;
				doOnce = true;
				WeaponPisto->OnReload(Mesh1P);

				// Load static asset
				FString ArmsPistoReloadMontage = FString(TEXT("AnimMontage'/Game/ShootingPawn/Animations/Arms_Glock_Reload_anim_Montage.Arms_Glock_Reload_anim_Montage'"));
				UAnimMontage* assetMontage = Cast<UAnimMontage>(LoadObject<UAnimMontage>(nullptr, *ArmsPistoReloadMontage));
				if (assetMontage != nullptr)
				{
					// Get the animation object for the arms mesh
					UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
					if (AnimInstance != nullptr)
					{
						AnimInstance->Montage_Play(assetMontage, 1.f);
					}
				}

				IsReloading = false;
				doOnce = false;
			}
		}
		else
		{
			// ��ʾ��ҩ����
		}
	}
}

void AShootingCharacter::OnAimDownSight()
{
	IsAimDown = true;
	hud->SetCrossWidgetVisible(false);
}

void AShootingCharacter::OnRecoverAimDownSight()
{
	IsAimDown = false;
	hud->SetCrossWidgetVisible(true);
}

/// Reload Sound
void AShootingCharacter::DropMagazine()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("DropMagazine"));
	// ����ж�µ�������
	FString DropSoundPath = FString(TEXT("SoundWave'/Game/ShootingPawn/Sounds/Reload_Drop.Reload_Drop'"));
	USoundBase* DropSound = Cast<USoundBase>(LoadObject<USoundBase>(nullptr, *DropSoundPath));
	if (DropSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
	}
	
	DropSound = nullptr;
}

void AShootingCharacter::InsertMagazine()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("InsertMagazine"));
	// ����װ�뵯������
	FString InsertSoundPath = FString(TEXT("SoundWave'/Game/ShootingPawn/Sounds/Reload_Insert.Reload_Insert'"));
	USoundBase* InsertSound = Cast<USoundBase>(LoadObject<USoundBase>(nullptr, *InsertSoundPath));
	if (InsertSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InsertSound, GetActorLocation());
	}

	InsertSound = nullptr;
}

void AShootingCharacter::ReloadMagazine()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("ReloadMagazine"));
	// �������ӵ�����
	FString ReloadSoundPath = FString(TEXT("SoundWave'/Game/ShootingPawn/Sounds/Reload_Load.Reload_Load'"));
	USoundBase* ReloadSound = Cast<USoundBase>(LoadObject<USoundBase>(nullptr, *ReloadSoundPath));
	if (ReloadSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}

	ReloadSound = nullptr;
}

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
