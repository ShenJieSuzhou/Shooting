// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingCharacter.h"
#include "ShootingProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);
PRAGMA_DISABLE_OPTIMIZATION

AShootingCharacter::~AShootingCharacter()
{
	CollisionActor = nullptr;
	hud = nullptr;
	CurrentWeapon = nullptr;
	WeaponRifle = nullptr;
	WeaponPisto = nullptr;
	WeaponKnife = nullptr;
}

PRAGMA_DISABLE_OPTIMIZATION
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

	OverlapCollision = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapCollision"));
	OverlapCollision->SetupAttachment(GetCapsuleComponent());
	OverlapCollision->InitSphereRadius(100.0f);
	OverlapCollision->BodyInstance.SetCollisionProfileName("WeaponCollision");
	OverlapCollision->OnComponentHit.AddDynamic(this, &AShootingCharacter::OnHit);
	OverlapCollision->OnComponentBeginOverlap.AddDynamic(this, &AShootingCharacter::OnSphereOverlap);
	OverlapCollision->OnComponentEndOverlap.AddDynamic(this, &AShootingCharacter::OnSphereEndOverlap);

	// Drop Weapon Point
	DropPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DropPoint"));
	DropPoint->SetupAttachment(GetCapsuleComponent());
	DropPoint->SetWorldLocation(FVector(70.f, 0.f, 40.f));

	// 初始化背包
	//inventory = NewObject<GunInventory>();
	//inventory->Initialize();

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
	//SetWeapons();
	WeaponRifle = NULL;
	WeaponPisto = NULL;
	WeaponKnife = NULL;

	doOnce = false;
	WeaponChanged = false;
	IsReloading = false;
	
	// 没有武器
	WeaponType = 3;
	CurWeaponType = EWeapon::EW_None;
	CurrOverlapWeapon = EWeapon::EW_None;
	IsAimDown = false;

	hud = Cast<AShootingHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
}

void AShootingCharacter::SetWeapons(EWeapon WeaponT)
{
	if(WeaponT == EWeapon::EW_Knife)
	{
		UClass* WeaponKnifeClass = LoadClass<AWeaponBase>(nullptr, TEXT("Class'/Script/Shooting.WeaponKnife'"));
		
		UWorld* const World = GetWorld();
		FVector Localtion = FVector(0.f, 0.f, 0.f);
		FRotator Rotator = FRotator(0.f);

		if (WeaponKnifeClass != nullptr)
		{
			if (World != nullptr)
			{
				WeaponKnife = Cast<AWeaponKnife>(World->SpawnActor<AWeaponBase>(WeaponKnifeClass, Localtion, Rotator));
				WeaponKnife->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Palm_R"));

				// 手持军刀
				this->OnHoldKnife();
			}
		}
	}
	else if(WeaponT == EWeapon::EW_AK)
	{
		UClass* WeaponRifleClass = LoadClass<AWeaponBase>(nullptr, TEXT("Class'/Script/Shooting.WeaponAK'"));

		UWorld* const World = GetWorld();
		FVector Localtion = FVector(0.f, 0.f, 0.f);
		FRotator Rotator = FRotator(0.f);

		if (WeaponRifleClass != nullptr)
		{
			if (World != nullptr)
			{
				WeaponRifle = Cast<AWeaponAK>(World->SpawnActor<AWeaponBase>(WeaponRifleClass, Localtion, Rotator));
				WeaponRifle->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Palm_R"));
				
				// 手持AK
				this->OnHoldRifle();
			}
		}
	}
	else if(WeaponT == EWeapon::EW_Pisto)
	{
		UClass* WeaponPistoClass = LoadClass<AWeaponBase>(nullptr, TEXT("Class'/Script/Shooting.WeaponGlock'"));

		UWorld* const World = GetWorld();
		FVector Localtion = FVector(0.f, 0.f, 0.f);
		FRotator Rotator = FRotator(0.f);

		if (WeaponPistoClass != nullptr)
		{
			if (World != nullptr)
			{
				WeaponPisto = Cast<AWeaponGlock>(World->SpawnActor<AWeaponBase>(WeaponPistoClass, Localtion, Rotator));
				WeaponPisto->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Palm_R"));
				
				// 手持Glock
				this->OnHoldPisto();
			}
		}
	}

	CurWeaponType = WeaponT;
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

	// Pickup Weapon
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AShootingCharacter::OnPickUp);

	// Drop Weapon
	PlayerInputComponent->BindAction("DropDown", IE_Pressed, this, &AShootingCharacter::OnDropDown);

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
	if(!WeaponRifle)
	{
		return;
	}

	if(!IsReloading)
	{
		CurrentWeapon = WeaponRifle;

		WeaponRifle->FP_Gun->SetHiddenInGame(false);
		if(WeaponPisto)
		{
			WeaponPisto->FP_Gun->SetHiddenInGame(true);
		}

		if(WeaponKnife)
		{
			WeaponKnife->FP_Gun->SetHiddenInGame(true);
		}
		
		CurWeaponType = EWeapon::EW_AK;
		WeaponType = 0;
		hud->UpdateAmmo(CurrentWeapon->AmmoCount, CurrentWeapon->MagazineAmmo, CurrentWeapon->MaxAmmoCount);
	}
}

void AShootingCharacter::OnHoldPisto()
{
	if (!WeaponPisto)
	{
		return;
	}

	if (!IsReloading)
	{
		CurrentWeapon = WeaponPisto;

		if(WeaponKnife)
		{
			WeaponKnife->FP_Gun->SetHiddenInGame(true);
		}

		if(WeaponRifle)
		{
			WeaponRifle->FP_Gun->SetHiddenInGame(true);
		}
		
		WeaponPisto->FP_Gun->SetHiddenInGame(false);
		CurWeaponType = EWeapon::EW_Pisto;
		WeaponType = 1;
		hud->UpdateAmmo(CurrentWeapon->AmmoCount, CurrentWeapon->MagazineAmmo, CurrentWeapon->MaxAmmoCount);
	}
}

void AShootingCharacter::OnHoldKnife()
{
	if (!WeaponKnife)
	{
		return;
	}

	if (!IsReloading)
	{
		WeaponChanged = false;
		CurrentWeapon = WeaponKnife;
		if(WeaponRifle)
		{
			WeaponRifle->FP_Gun->SetHiddenInGame(true);
		}
		
		if(WeaponPisto)
		{
			WeaponPisto->FP_Gun->SetHiddenInGame(true);
		}
		
		WeaponKnife->FP_Gun->SetHiddenInGame(false);
		CurWeaponType = EWeapon::EW_Knife;
		WeaponType = 2;
		hud->UpdateAmmo(0, 0, 0);
	}
}

void AShootingCharacter::OnFire()
{
	if (CurWeaponType == EWeapon::EW_None)
	{
		return;
	}

	if(CurWeaponType == EWeapon::EW_Knife)
	{
		if (!WeaponKnife)
		{
			return;
		}
		WeaponKnife->OnFire(Mesh1P);
	}
	else if(CurWeaponType == EWeapon::EW_AK)
	{
		if (!WeaponRifle)
		{
			return;
		}
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
		if (!WeaponPisto)
		{
			return;
		}
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
	if (CurWeaponType == EWeapon::EW_None)
	{
		return;
	}

	if (CurWeaponType == EWeapon::EW_Knife)
	{
		// Knife need not reload
	}
	else if (CurWeaponType == EWeapon::EW_AK)
	{
		if(!WeaponRifle)
		{
			return;
		}

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
			// 提示弹药不足
		}
	}
	else if (CurWeaponType == EWeapon::EW_Pisto)
	{
		if (!WeaponPisto)
		{
			return;
		}
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
			// 提示弹药不足
		}
	}
}

void AShootingCharacter::OnPickUp()
{
	if (CurrOverlapWeapon == EWeapon::EW_None)
	{
		return;
	}

	CollisionActor->Destroy();
	this->SetWeapons(CurrOverlapWeapon);
	hud->AmmoWidget->UpdateWeaponsInventory(WeaponRifle, WeaponPisto, WeaponKnife);
	CurrOverlapWeapon = EWeapon::EW_None;
}

void AShootingCharacter::OnDropDown()
{	
	// Drop Current Weapon
	if (CurWeaponType == EWeapon::EW_None)
	{
		return;
	}

	FVector Localtion = DropPoint->GetComponentLocation();
	FVector Scale = DropPoint->GetComponentScale();
	float RandValue = FMath::FRandRange(-180, 180);
	FRotator Rotator = FRotator(RandValue);
	FVector ForwardVector = DropPoint->GetForwardVector() * 500;

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	if(CurWeaponType == EWeapon::EW_AK)
	{
		UClass* WeaponRifleClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/PickUp_AK_BP.PickUp_AK_BP_C'"));
		
		if (WeaponRifleClass != nullptr)
		{
			AActor* Rifle = World->SpawnActor<AActor>(WeaponRifleClass, Localtion, Rotator);
			UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(Rifle->GetRootComponent());
			if (SM)
			{
				SM->AddImpulse(ForwardVector, NAME_None, true);
			}
		}

		WeaponRifle->Destroy();
		WeaponRifle = nullptr;
		WeaponType = 3;
		CurWeaponType = EWeapon::EW_None;
	}
	else if(CurWeaponType == EWeapon::EW_Pisto)
	{
		UClass* WeaponPistoClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/PickUp_Glock_BP.PickUp_Glock_BP_C'"));
		
		if (WeaponPistoClass != nullptr)
		{
			AActor* Pisto = World->SpawnActor<AActor>(WeaponPistoClass, Localtion, Rotator);
			UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(Pisto->GetRootComponent());
			if (SM)
			{
				SM->AddImpulse(ForwardVector, NAME_None, true);
			}

			WeaponPisto->Destroy();
			WeaponPisto = nullptr;
			WeaponType = 3;
			CurWeaponType = EWeapon::EW_None;
		}
	}
	else if(CurWeaponType == EWeapon::EW_Knife)
	{
		UClass* WeaponKnifeClass = LoadClass<AActor>(nullptr, TEXT("Blueprint'/Game/ShootingPawn/Blueprints/PickUp_Knife_BP.PickUp_Knife_BP_C'"));
	
		if (WeaponKnifeClass != nullptr)
		{
			AActor* Knife = World->SpawnActor<AActor>(WeaponKnifeClass, Localtion, Rotator);
			UStaticMeshComponent* SM = Cast<UStaticMeshComponent>(Knife->GetRootComponent());
			if (SM)
			{
				SM->AddImpulse(ForwardVector, NAME_None, true);
			}

			WeaponKnife->Destroy();
			WeaponKnife = nullptr;
			WeaponType = 3;
			CurWeaponType = EWeapon::EW_None;
		}
	}
}


void AShootingCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}


void AShootingCharacter::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OterComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<APickUpWeaponBase>(OtherActor) == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("NULL"));
		return;
	}

	CollisionActor = OtherActor;
	APickUpWeaponBase* Weapon = Cast<APickUpWeaponBase>(OtherActor);
	CurrOverlapWeapon = Weapon->WeaponType;
	hud->AmmoWidget->ShowTip(CurrOverlapWeapon);
}

void AShootingCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APickUpWeaponBase>(OtherActor) == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("NULL"));
		return;
	}

	APickUpWeaponBase* Weapon = Cast<APickUpWeaponBase>(OtherActor);
	hud->AmmoWidget->HiddenTip();
}



void AShootingCharacter::OnAimDownSight()
{
	if (CurWeaponType == EWeapon::EW_None)
	{
		return;
	}

	IsAimDown = true;
	hud->SetCrossWidgetVisible(false);
}

void AShootingCharacter::OnRecoverAimDownSight()
{
	if (CurWeaponType == EWeapon::EW_None)
	{
		return;
	}

	IsAimDown = false;
	hud->SetCrossWidgetVisible(true);
}

/// Reload Sound
void AShootingCharacter::DropMagazine()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("DropMagazine"));
	// 播放卸下弹夹声音
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
	// 播放装入弹夹声音
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
	// 播放上子弹声音
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
