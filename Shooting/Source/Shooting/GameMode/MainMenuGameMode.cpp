// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

AMainMenuGameMode::AMainMenuGameMode()
{

}

void AMainMenuGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void AMainMenuGameMode::StartPlay()
{
	Super::StartPlay();
	
}

void AMainMenuGameMode::PreLoadMaterials()
{
	FShootingMaterialUtil::GetInstance()->GetResLoadDelegate().BindUObject(this, &AMainMenuGameMode::OnMaterialLoaded);
	FShootingMaterialUtil::GetInstance()->PreloadMaterials();
}


void AMainMenuGameMode::OnMaterialLoaded(bool result)
{
	if(result)
	{
		// Òþ²Ø Loading ²¢¿ªÆô StartGame °´Å¥
		AssetsLoadComplete();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("22222"));
	}
}
