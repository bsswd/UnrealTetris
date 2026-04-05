// Unreal Tetris Game. Made by Alex Sinkin. (c)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UT_Brick.generated.h"


UCLASS()
class UNREALTETRIS_API UUT_Brick : public UObject
{
	GENERATED_BODY()
	
public:
	
	UUT_Brick();
	
	FVector2D CurrentCoords;
	FVector2D OldCoords;
	
	TObjectPtr<UStaticMeshComponent> Brick;
	
	void SetPoint(FVector2D Point, bool bSaveCurrentPosition);
	void RotateBrick(FVector2D Center, bool bIsRight, bool bSaveCurrentPosition);
	void RestorePoint();
	void ApplyPoint();
};
