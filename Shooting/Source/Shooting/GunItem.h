// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "GunItem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTING_API UGunItem : public UUserWidget
{
	GENERATED_BODY()
	~UGunItem();
public:
	void SetImage(UTexture2D* selected, UTexture2D* UnSelected);

	void SetIsOwned(bool isOwned);

public:
	UPROPERTY(Meta=(BindWidget))
	class UBorder* Background;
	
	UPROPERTY(BlueprintReadWrite, Meta=(BindWidget))
	class UImage* WeaponImage;

private:
	UTexture2D* Owned;
	UTexture2D* NotOwned;
};
