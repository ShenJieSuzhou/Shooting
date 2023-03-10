// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "GunInventory.h"
#include "GunItem.h"
#include "WeaponBase.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTING_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* MagazineAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TotalAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WeaponTip;

	// Ç¹Ðµ±³°üÖ¸Ê¾
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UGunItem* RifleLight;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UGunItem* PistoLight;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UGunItem* KnifeLight;

public:
	void InitItems();

	void UpdateAmmo(int current, int Magazine, int maxAmmount);

	void UpdateWeaponsInventory(GunInventory* inventory);

	void ShowTip(EWeapon weaponType);

	void HiddenTip();
};
