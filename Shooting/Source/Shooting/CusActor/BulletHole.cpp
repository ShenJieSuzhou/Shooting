// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletHole.h"
#include "Materials/MaterialInstanceConstant.h"
// Sets default values
ABulletHole::ABulletHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABulletHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletHole::SetBulletHoleMaterial(FString path)
{
	if(Decal)
	{
		// º”‘ÿ≤ƒ÷ 
		UMaterialInstanceConstant* MaterialInterface = LoadObject<UMaterialInstanceConstant>(NULL, *path);
		//UMaterialInstanceConstant* MaterialInterface = Cast<UMaterialInstanceConstant>(StaticLoadObject(UMaterialInstanceConstant::StaticClass(), nullptr, *path));
		if (MaterialInterface)
		{
			UMaterialInterface* DecalMaterial = MaterialInterface;
			if (DecalMaterial)
			{
				Decal->SetDecalMaterial(DecalMaterial);
				Decal->DecalSize = FVector(10.f, 10.f, 10.f);
			}
		}
		

		//UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(StaticLoadObject(UMaterialInstanceDynamic::StaticClass(), nullptr, *path));
		//UMaterial* MatObj = LoadObject<UMaterial>(NULL, *path);
		//UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(MatObj, this);
	}
}

