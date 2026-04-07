// Unreal Tetris Game. Made by Alex Sinkin. (c)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UT_MainWidget.generated.h"

class UWidgetSwitcher;
class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSimpleActionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveActionDelegate, float, Direction);


UCLASS()
class UNREALTETRIS_API UUT_MainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Score;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Level;
	
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;
	
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> StartButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> QuitButton;
	
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> LeftButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RightButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> DropButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RotateButton;
	
	
	UPROPERTY(BlueprintAssignable, Category = "UT|Events")
	FOnSimpleActionDelegate OnStartClicked;
	
	UPROPERTY(BlueprintAssignable, Category = "UT|Events")
	FOnSimpleActionDelegate OnQuitClicked;
	
	UPROPERTY(BlueprintAssignable, Category = "UT|Events")
	FOnSimpleActionDelegate OnRotateClicked;
 
	UPROPERTY(BlueprintAssignable, Category = "UT|Events")
	FOnSimpleActionDelegate OnDropPressed;
 
	UPROPERTY(BlueprintAssignable, Category = "UT|Events")
	FOnSimpleActionDelegate OnDropReleased;
 
	UPROPERTY(BlueprintAssignable, Category = "UT|Events")
	FOnMoveActionDelegate OnMoveClicked;
	
	
	UFUNCTION()
	void OnStartButtonTap();

	UFUNCTION()
	void OnQuitButtonTap();
	
	UFUNCTION()
	void OnLeftButtonTap();
	
	UFUNCTION()
	void OnRightButtonTap();
	
	UFUNCTION()
	void OnDropButtonTap();
	
	UFUNCTION()
	void OnDropButtonRelease();
	
	UFUNCTION()
	void OnRotateButtonTap();
	
	UFUNCTION()
	void SwitchToGame();
	
	UFUNCTION()
	void SwitchToMenu();
	
	
	UFUNCTION()
	void UpdateScore(const int InScore);
	
	UFUNCTION()
	void UpdateLevel(const int InLevel);
	
	
protected:
	
	virtual void NativeConstruct() override;
};