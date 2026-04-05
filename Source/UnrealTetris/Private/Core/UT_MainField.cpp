// Unreal Tetris Game. Made by Alex Sinkin. (c)

#include "UnrealTetris/Public/Core/UT_MainField.h"


AUT_MainField::AUT_MainField()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MainScene = CreateDefaultSubobject<USceneComponent>("MainScene");
	RootComponent = MainScene;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(MainScene);
	
}

void AUT_MainField::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUT_MainField::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


