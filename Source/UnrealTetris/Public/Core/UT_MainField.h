// Unreal Tetris Game. Made by Alex Sinkin. (c)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "UT_MainField.generated.h"

#define WIDTHCELLS 10
#define HEIGHTCELLS 20

class UUT_MainWidget;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
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
	
	
	UPROPERTY(EditAnywhere, Category="UT|UI")
	TSubclassOf<UUT_MainWidget> MainWidgetClass;
 
	UPROPERTY()
	TObjectPtr<UUT_MainWidget> MainWidget;
	
	
	TArray<TObjectPtr<UStaticMeshComponent>> Field;
	
	void MoveVerticalByTimer();
	void MoveHorizontalByTimer();
	void ReleaseFigure();
	
	void CheckRow();
	void DeleteRow();
	

protected:
	
	UPROPERTY()
	TArray<TObjectPtr<UUT_Brick>> NextFigure;
	
	UPROPERTY()
	TArray<TObjectPtr<UUT_Brick>> CurrentFigure;
	
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstance>> BrickColors;
	
	UPROPERTY()
	TArray<int32> FullRows;
	
	bool bIsGameInProgress;
	
	FTimerHandle MoveVerticalTimerHandle;
	FTimerHandle MoveHorizontalTimerHandle;
	FTimerHandle ClearRowTimerHandle;
	
	int32 CurrentLevel;
	int32 CurrentSpeed;
	
	
	bool CheckCurrentFigurePosition();
	bool CheckNewFigurePosition();
	
	void MoveVerticalCurrentFigure(int32 Amount, bool bBottom = true, bool bSaveCurrentPosition = false);
	void MoveHorizontalCurrentFigure(int32 Amount, bool bRight =true,	bool bSaveCurrentPosition = false);
	
	int32 GetMostLeftBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	int32 GetMostBottomBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	int32 GetMostRightBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	int32 GetMostTopBrick(TArray<TObjectPtr<UUT_Brick>>& Figure);
	
	void ClearField();
	void InitField();
	
	void CreateNextFigure();
	void StartFigure();
	
	void SetFigurePoint(FVector2D Point, TArray<TObjectPtr<UUT_Brick>>& Figure, bool bSaveCurrentPosition = false);
	void ApplyFigurePoint(TArray<TObjectPtr<UUT_Brick>>& Figure);
	void RestorePoint();
	
	void RotateFigure(TArray<TObjectPtr<UUT_Brick>>& Figure, bool bRight = true, bool bSaveCurrentPosition = false);

	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
private:
	
	UPROPERTY(EditAnywhere, Category = "UT|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, Category = "UT|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "UT|Input")
	TObjectPtr<UInputAction> DropAction;
	
	UPROPERTY(EditAnywhere, Category = "UT|Input")
	TObjectPtr<UInputAction> RotateAction;
	
	
	UPROPERTY(EditAnywhere, Category = "UT|Gameplay")
	TArray<float> Speed;
	
	
	UPROPERTY(EditAnywhere, Category = "UT|Sound")
	USoundBase* ClearRowSound;
	
	UPROPERTY(EditAnywhere, Category = "UT|Sound")
	USoundBase* DropDownSound;
	
	UPROPERTY(EditAnywhere, Category = "UT|Sound")
	USoundBase* LevelUpSound;
	
	UPROPERTY(EditAnywhere, Category = "UT|Sound")
	USoundBase* MoveSound;
	
	UPROPERTY(EditAnywhere, Category = "UT|Sound")
	USoundBase* RotateSound;
	
	
	int32 Score = 0;
	
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	UFUNCTION()
	void MoveFromUI(float Direction);
	
	UFUNCTION()
	void Drop();
	
	UFUNCTION()
	void DropRelease();
	
	UFUNCTION()
	void Rotate();
	
	UFUNCTION()
	void OnStartGame();
	
	UFUNCTION()
	void OnGameOver();
};