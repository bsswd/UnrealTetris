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
	

protected:
	
	UPROPERTY()
	TArray<TObjectPtr<UUT_Brick>> NextFigure;
	
	UPROPERTY()
	TArray<TObjectPtr<UUT_Brick>> CurrentFigure;
	
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstance>> BrickColors; 
	
	void ClearField();
	void InitField();
	
	void CreateNextFigure();
	void StartFigure();
	
	void SetPointFigure(FVector2D Point, TArray<TObjectPtr<UUT_Brick>>& Figure, bool bSaveCurrentPosition = false);
	void ApplyPointFigure(TArray<TObjectPtr<UUT_Brick>>& Figure);
	

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};