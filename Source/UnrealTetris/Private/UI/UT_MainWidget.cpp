// Unreal Tetris Game. Made by Alex Sinkin. (c)


#include "UI/UT_MainWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"


void UUT_MainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UUT_MainWidget::OnStartButtonTap);
	}
	
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UUT_MainWidget::OnQuitButtonTap);
	}
	
	if (LeftButton)
	{
		LeftButton->OnClicked.AddDynamic(this, &UUT_MainWidget::OnLeftButtonTap);
	}
	
	if (RightButton)
	{
		RightButton->OnClicked.AddDynamic(this, &UUT_MainWidget::OnRightButtonTap);
	}
	
	if (DropButton)
	{
		DropButton->OnPressed.AddDynamic(this, &UUT_MainWidget::OnDropButtonTap);
		DropButton->OnReleased.AddDynamic(this, &UUT_MainWidget::OnDropButtonRelease);
	}
	
	if (RotateButton)
	{
		RotateButton->OnClicked.AddDynamic(this, &UUT_MainWidget::OnRotateButtonTap);
	}
}

void UUT_MainWidget::OnStartButtonTap()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnStartButtonTap()"));
	SwitchToGame();
	OnStartClicked.Broadcast();
}

void UUT_MainWidget::OnQuitButtonTap()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnQuitButtonTap()"));
	OnQuitClicked.Broadcast();
}

void UUT_MainWidget::OnLeftButtonTap()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnLeftButtonTap()"));
	OnMoveClicked.Broadcast(-1.0f);
}

void UUT_MainWidget::OnRightButtonTap()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnRightButtonTap()"));
	OnMoveClicked.Broadcast(1.0f);
}

void UUT_MainWidget::OnDropButtonTap()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnDropButtonTap()"));
	OnDropPressed.Broadcast();
}

void UUT_MainWidget::OnDropButtonRelease()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnDropButtonRelease()"));
	OnDropReleased.Broadcast();
}

void UUT_MainWidget::OnRotateButtonTap()
{
	UE_LOG(LogTemp, Warning, TEXT("UUT_MainWidget::OnRotateButtonTap()"));
	OnRotateClicked.Broadcast();
}

void UUT_MainWidget::SwitchToGame()
{
	Switcher->SetActiveWidgetIndex(0);
}

void UUT_MainWidget::SwitchToMenu()
{
	Switcher->SetActiveWidgetIndex(1);
}