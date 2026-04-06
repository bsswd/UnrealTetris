// Unreal Tetris Game. Made by Alex Sinkin. (c)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UT_MainField.generated.h"

#define WIDTHCELLS 10
#define HEIGHTCELLS 20

class UStaticMeshComponent;
class UCameraComponent;
class UUT_Brick;


UCLASS()
class UNREALTETRIS_API AUT_MainField : public APawn
{
	GENERATED_BODY()

public:
	
	AUT_MainField();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MainScene;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere)
    TObjectPtr<USceneComponent> Borders;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> LeftBorder;
	
	UPROPERTY(EditAnywhere)
    TObjectPtr<UStaticMeshComponent> RightBorder;
    
    UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> BottomBorder;
	
	
	TArray<TObjectPtr<UStaticMeshComponent>> Field;
	
	void MoveDownByTimer();
	void ReleaseFigure();
	

protected:
	
	UPROPERTY()
	TArray<TObjectPtr<UUT_Brick>> NextFigure;
	
	UPROPERTY()
	TArray<TObjectPtr<UUT_Brick>> CurrentFigure;
	
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstance>> BrickColors; 
	
	bool bIsGameInProgress;
	
	FTimerHandle MoveDownTimerHandle;
	FTimerHandle MoveRightTimerHandle;
	FTimerHandle MoveLeftTimerHandle;
	FTimerHandle ClearRowTimerHandle;
	
	bool CheckCurrentFigurePosition();
	bool CheckNewFigurePosition();
	
	void MoveVerticalCurrentFigure(int Amount, bool bBottom = true, bool bSaveCurrentPosition = false);
	void MoveHorizontalCurrentFigure(int Amount, bool bRight =true,	bool bSaveCurrentPosition = false);
	
	int GetMostLeftBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	int GetMostBottomBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	int GetMostRightBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	int GetMostTopBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	
	void ClearField();
	void InitField();
	
	void CreateNextFigure();
	void StartFigure();
	
	void SetFigurePoint(FVector2D Point, TArray<TObjectPtr<UUT_Brick>>& Figure, bool bSaveCurrentPosition = false);
	void ApplyFigurePoint(TArray<TObjectPtr<UUT_Brick>>& Figure);
	void RestorePoint();
	
	void RotateFigure(TArray<TObjectPtr<UUT_Brick>>& Figure, bool bRight = true, bool bSaveCurrentPosition = false);
	

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};