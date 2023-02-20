// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingGameMode.h"
#include "ShootingHUD.h"
#include "ShootingCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShootingGameMode::AShootingGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/ShootingPawn/Blueprints/FPSPlayer_BP.FPSPlayer_BP'"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AShootingHUD::StaticClass();
}
