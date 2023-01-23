// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "CrossHairWidget.h"
#include "UObject/ConstructorHelpers.h"

AShootingHUD::AShootingHUD()
{
	// Set the crosshair texture
	//static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	//CrosshairTex = CrosshairTexObj.Object;
}


void AShootingHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	//const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	//// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	//const FVector2D CrosshairDrawPosition( (Center.X),
	//									   (Center.Y + 20.0f));

	//// draw the crosshair
	//FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	//TileItem.BlendMode = SE_BLEND_Translucent;
	//Canvas->DrawItem( TileItem );
}


void AShootingHUD::BeginPlay()
{
	Super::BeginPlay();

	FString WidgetClassLoadPath = FString(TEXT("WidgetBlueprint'/Game/ShootingPawn/Blueprints/XHair_HUD.XHair_HUD_C'"));
	TSubclassOf<UCrossHairWidget> LoginWidgetClass = LoadClass<UCrossHairWidget>(NULL, *WidgetClassLoadPath);
	UCrossHairWidget* LoginWidget = CreateWidget<UCrossHairWidget>(GetWorld(), LoginWidgetClass);
	LoginWidget->AddToViewport();
}