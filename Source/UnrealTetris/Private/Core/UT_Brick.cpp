// Unreal Tetris Game. Made by Alex Sinkin. (c)


#include "Core/UT_Brick.h"

#include "ViewportInteractionTypes.h"

UUT_Brick::UUT_Brick()
{
	CurrentCoords = OldCoords = FVector2D::ZeroVector;
}

void UUT_Brick::SetPoint(FVector2D Point, bool bSaveCurrentPosition)
{
	if (bSaveCurrentPosition)
	{
		OldCoords = CurrentCoords;
	}
	
	CurrentCoords += Point;
}

void UUT_Brick::RotateBrick(FVector2D Center, bool bIsRight, bool bSaveCurrentPosition)
{
	if (bSaveCurrentPosition)
	{
		OldCoords = CurrentCoords;
	}
	
	FVector2D TempCoords = FVector2D(CurrentCoords.Y - Center.Y, CurrentCoords.X - Center.X);
	
	float Cf = bIsRight ? 1.f : -1.f; 
	
	CurrentCoords = FVector2D(Center.X - (Cf * TempCoords.X), Center.Y + (Cf * TempCoords.Y));
}

void UUT_Brick::RestorePoint()
{
	CurrentCoords = OldCoords;
}

void UUT_Brick::ApplyPoint()
{
	if (Brick != nullptr)
	{
		Brick->SetRelativeLocation(FVector(CurrentCoords.X * 100.f, 0.f, CurrentCoords.Y * 100.f));
	}
}
