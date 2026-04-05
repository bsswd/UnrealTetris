// Unreal Tetris Game. Made by Alex Sinkin. (c)

#include "UnrealTetris/Public/Core/UT_MainField.h"


AUT_MainField::AUT_MainField()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUT_MainField::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUT_MainField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

