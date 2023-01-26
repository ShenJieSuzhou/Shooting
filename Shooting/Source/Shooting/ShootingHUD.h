// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrossHairWidget.h"
#include "AmmoWidget.h"
#include "ShootingHUD.generated.h"

UCLASS()
class AShootingHUD : public AHUD
{
	GENERATED_BODY()

public:
	UAmmoWidget* AmmoWidget;
public:
	AShootingHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void UpdateAmmo(int current, int Magazine, int maxAmmount);
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

