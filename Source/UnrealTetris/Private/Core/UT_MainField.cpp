// Unreal Tetris Game. Made by Alex Sinkin. (c)

#include "UnrealTetris/Public/Core/UT_MainField.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/UT_Brick.h"
#include "Kismet/GameplayStatics.h"
#include "UI/UT_MainWidget.h"
#include "UObject/ConstructorHelpers.h"

#define CYCLEFOR(I) for (int32 i = 0; i < I; i++)

int32 FigureArr[7][4] = 
	{
	{0, 2, 4, 6}, // I
	{0, 2, 3, 5}, // S
	{1, 2, 3, 4}, // Z
	{0, 1, 2, 3}, // 0
	{0, 2, 3, 4}, // T
	{0, 2, 4, 5}, // L
	{1, 3, 4, 5}, // J
	}; 


AUT_MainField::AUT_MainField()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MainScene = CreateDefaultSubobject<USceneComponent>("MainScene");
	RootComponent = MainScene;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(MainScene);
	Camera->bConstrainAspectRatio = true;
	Camera->AspectRatio = 9.0f / 16.0f;
	Camera->SetRelativeLocation(FVector(500.f, 25000.f, 700.f));
	Camera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Camera->SetFieldOfView(4.5f);
	
	Borders = CreateDefaultSubobject<USceneComponent>("Borders");
	Borders->SetupAttachment(MainScene);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	
	LeftBorder = CreateDefaultSubobject<UStaticMeshComponent>("LeftBorder");
	LeftBorder->SetRelativeLocation(FVector(-100.f, 0.f, 900.f));
	LeftBorder->SetRelativeScale3D(FVector(1.f, 1.f, 21.f));
	LeftBorder->SetStaticMesh(CubeVisualAsset.Object);
	LeftBorder->SetupAttachment(Borders);
	
	RightBorder = CreateDefaultSubobject<UStaticMeshComponent>("RightBorder");
	RightBorder->SetRelativeLocation(FVector(1000.f, 0.f, 900.f));
	RightBorder->SetRelativeScale3D(FVector(1.f, 1.f, 21.f));
	RightBorder->SetStaticMesh(CubeVisualAsset.Object);
	RightBorder->SetupAttachment(Borders);
	
	BottomBorder = CreateDefaultSubobject<UStaticMeshComponent>("BottomBorder");
	BottomBorder->SetRelativeLocation(FVector(450.f, 0.f, -100.f));
	BottomBorder->SetRelativeScale3D(FVector(12.f, 1.f, 1.f));
	BottomBorder->SetStaticMesh(CubeVisualAsset.Object);
	BottomBorder->SetupAttachment(Borders);
	
	InitField();
	
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color1.MI_Color1")).Object);
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color2.MI_Color2")).Object);
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color3.MI_Color3")).Object);
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color4.MI_Color4")).Object);
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color5.MI_Color5")).Object);
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color6.MI_Color6")).Object);
	BrickColors.Add(ConstructorHelpers::FObjectFinder<UMaterialInstance>(TEXT("/Game/Materials/MI_Color7.MI_Color7")).Object);
	
	Speed.Add(1.f);
	Speed.Add(0.9f);
	Speed.Add(0.8f);
	Speed.Add(0.7f);
	Speed.Add(0.6f);
	Speed.Add(0.5f);
	Speed.Add(0.4f);
	Speed.Add(0.3f);
	Speed.Add(0.2f);
	Speed.Add(0.04f);
	
	bIsGameInProgress = false;
	
	CurrentLevel = 0;
	CurrentSpeed = 0;
}

void AUT_MainField::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!MainWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::BeginPlay() --- MainWidgetClass is null!"))
		return;
	}
	
	MainWidget = CreateWidget<UUT_MainWidget>(PC, MainWidgetClass);
	
	if (!MainWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::BeginPlay() --- MainWidget is null!"))
		return;
	}
	
	MainWidget->AddToViewport();
	
	MainWidget->OnRotateClicked.AddDynamic(this, &AUT_MainField::Rotate);
	MainWidget->OnDropPressed.AddDynamic(this, &AUT_MainField::Drop);
	MainWidget->OnDropReleased.AddDynamic(this, &AUT_MainField::DropRelease);
	
	MainWidget->OnMoveClicked.AddDynamic(this, &AUT_MainField::MoveFromUI);
	
	MainWidget->SwitchToGame();
	
	
	SetActorLocation(FVector(0.f));
	SetActorRotation(FRotator(0.f));
	
	ClearField();
	
	CreateNextFigure();
	StartFigure();
	CreateNextFigure();
		
	bIsGameInProgress = true;
	
	GetWorldTimerManager().SetTimer(MoveVerticalTimerHandle,
									this,
									&AUT_MainField::MoveVerticalByTimer,
									Speed[CurrentSpeed],
									true,
									Speed[CurrentSpeed]);
}

/** ------------------------------ INPUT ---------------------------------------------- **/

void AUT_MainField::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()->GetFirstLocalPlayerFromController());
	if (!IsValid(InputSubsystem)) return;
	
	InputSubsystem->AddMappingContext(InputMappingContext, 0);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!IsValid(EnhancedInputComponent)) return;
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &ThisClass::Drop);
	EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Completed, this, &ThisClass::DropRelease);
	EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Started, this, &ThisClass::Rotate);
}

void AUT_MainField::Move(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::Move()"));
	
	if (!bIsGameInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::Move() --- Game not in progress"));
		return;
	}
	
	const float MoveValue = Value.Get<float>();
 
	if (FMath::IsNearlyZero(MoveValue)) return;
	
	bool bMoveRight = MoveValue > 0.f;
	
	MoveHorizontalCurrentFigure(1, bMoveRight, true);
	
	if (CheckCurrentFigurePosition())
	{
		RestorePoint();
	}
	else
	{
		ApplyFigurePoint(CurrentFigure);
	}
	
	if (!MoveSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::Move() --- No MoveSound!"));
		return;
	}
	
	UGameplayStatics::PlaySound2D(this, MoveSound);
}

void AUT_MainField::MoveFromUI(float Direction)
{
	FInputActionValue FakeValue(Direction);
	Move(FakeValue);
}

void AUT_MainField::Drop()
{
	CurrentSpeed = Speed.Num() - 1;
	
	GetWorldTimerManager().SetTimer(MoveVerticalTimerHandle,
									this,
									&AUT_MainField::MoveVerticalByTimer,
									Speed[CurrentSpeed],
									true,
									0.f);
}

void AUT_MainField::DropRelease()
{
	CurrentSpeed = CurrentLevel;
	
	GetWorldTimerManager().SetTimer(MoveVerticalTimerHandle,
									this,
									&AUT_MainField::MoveVerticalByTimer,
									Speed[CurrentSpeed],
									true,
									Speed[CurrentSpeed]);
}

void AUT_MainField::Rotate()
{
	RotateFigure(CurrentFigure);
	
	if (!RotateSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::Rotate() --- No RotateSound!"));
		return;
	}
	UGameplayStatics::PlaySound2D(this, RotateSound);
}


/** ------------------------------ FIELD ---------------------------------------------- **/

void AUT_MainField::ClearField()
{	
	for (int32 i = 0; i < WIDTHCELLS * HEIGHTCELLS; i++)
	{
		if (Field[i] != nullptr)
		{
			Field[i]->DestroyComponent();
			Field[i] = nullptr;
		}
	}
	
	CYCLEFOR(4)
	{
		if (CurrentFigure[i]->Brick != nullptr)
		{
			CurrentFigure[i]->Brick->DestroyComponent();
			CurrentFigure[i]->Brick = nullptr;
		}
		
		if (NextFigure[i]->Brick != nullptr)
		{
			NextFigure[i]->Brick->DestroyComponent();
			NextFigure[i]->Brick = nullptr;
		}
	}
}

void AUT_MainField::InitField()
{
	Field.Reserve(WIDTHCELLS * HEIGHTCELLS + 1);
	
	for (int32 i = 0; i < WIDTHCELLS * HEIGHTCELLS; i++)
	{
		Field.Add(nullptr);
	}
	
	CurrentFigure.Reserve(5);
	NextFigure.Reserve(5);
	
	CYCLEFOR(4)
	{
		FName CurrentBrickName = *FString::Printf(TEXT("CurrentBrick_%d"), i);
		FName NextBrickName = *FString::Printf(TEXT("NextBrick_%d"), i);
		
		CurrentFigure.Add(CreateDefaultSubobject<UUT_Brick>(CurrentBrickName));
		NextFigure.Add(CreateDefaultSubobject<UUT_Brick>(NextBrickName));
	}
}

/** ------------------------------ FIGURE ---------------------------------------------- **/

void AUT_MainField::CreateNextFigure()
{
	int32 NextElement = FMath::Rand() % 7;
	
	CYCLEFOR(4)
	{
		NextFigure[i]->CurrentCoords.X = FigureArr[NextElement][i] % 2;
		NextFigure[i]->CurrentCoords.Y = FigureArr[NextElement][i] / 2;
		
		NextFigure[i]->Brick = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
		NextFigure[i]->Brick->RegisterComponent();
		NextFigure[i]->Brick->AttachToComponent(Borders, FAttachmentTransformRules::SnapToTargetIncludingScale);
		NextFigure[i]->Brick->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
		NextFigure[i]->Brick->SetMaterial(0, BrickColors[NextElement]);
		NextFigure[i]->Brick->SetRelativeRotation(FRotator(0.f));
		NextFigure[i]->Brick->SetRelativeScale3D(FVector(0.9f));
	}
	
	int32 NextRotation = FMath::Rand() % 6;
	
	CYCLEFOR(NextRotation)
	{
		RotateFigure(NextFigure);
	}
	
	SetFigurePoint(FVector2D(12.f, 12.f), NextFigure, false);
	ApplyFigurePoint(NextFigure);
}

void AUT_MainField::StartFigure()
{
	CYCLEFOR(4)
	{
		CurrentFigure[i]->CurrentCoords = NextFigure[i]->CurrentCoords;
		CurrentFigure[i]->Brick = NextFigure[i]->Brick;
		
		NextFigure[i]->Brick = nullptr;
	}
	
	SetFigurePoint(FVector2D(4.f, 18.f), CurrentFigure);
	
	while (CheckNewFigurePosition())
	{
		MoveVerticalCurrentFigure(1, false);
		bIsGameInProgress = false;
		GetWorld()->GetTimerManager().PauseTimer(MoveVerticalTimerHandle);
	}
	
	ApplyFigurePoint(CurrentFigure);
}

void AUT_MainField::SetFigurePoint(FVector2D Point, TArray<TObjectPtr<UUT_Brick>>& Figure,  bool bSaveCurrentPosition)
{
	FVector2D DeltaPoint = FVector2D(Point.X - Figure[GetMostLeftBrick(Figure)]->CurrentCoords.X,
										Point.Y - Figure[GetMostBottomBrick(Figure)]->CurrentCoords.Y);
	
	CYCLEFOR(4)
	{
		Figure[i]->SetPoint(DeltaPoint, bSaveCurrentPosition);	
	}
}

void AUT_MainField::ApplyFigurePoint(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	CYCLEFOR(4)
	{
		Figure[i]->ApplyPoint();
	}
}

void AUT_MainField::RestorePoint()
{
	CYCLEFOR(4)
	{
		CurrentFigure[i]->RestorePoint();
	}
}

void AUT_MainField::ReleaseFigure()
{
	if (CurrentFigure[GetMostTopBrick(CurrentFigure)]->CurrentCoords.Y >= HEIGHTCELLS)
	{
		bIsGameInProgress = false;
		GetWorldTimerManager().PauseTimer(MoveVerticalTimerHandle);
	}
	else
	{
		CYCLEFOR(4)
		{
			Field[WIDTHCELLS * CurrentFigure[i]->CurrentCoords.Y + CurrentFigure[i]->CurrentCoords.X] = CurrentFigure[i]->Brick;
		}
		
		StartFigure();
		CreateNextFigure();
	}
	
	CheckRow();
	
	if (!DropDownSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::ReleaseFigure() --- No DropDownSound!"));
		return;
	}
		
	UGameplayStatics::PlaySound2D(this, DropDownSound);
}

void AUT_MainField::DeleteRow()
{
	static int32 Count = 0;
	static float Cf = 0.f;
	
	if (Count == 6)
	{
		Count = 0;
	}
	
	++Count;
	
	if (Count < 6)
	{
		Cf = 1 - Count * 0.2f;
		
		for (int32 ElementIndex = 0; ElementIndex < FullRows.Num(); ++ElementIndex)
		{
			int32 Row = FullRows[ElementIndex];
			
			for (int32 Col = 0; Col < WIDTHCELLS; ++Col)
			{
				if (Cf == 0)
				{
					Field[WIDTHCELLS * Row + Col]->DestroyComponent();
					Field[WIDTHCELLS * Row + Col] = nullptr;
					
					if (!ClearRowSound)
					{
						UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::DeleteRow() --- No ClearRowSound!"))
						return;
					}
					
					UGameplayStatics::PlaySound2D(this, ClearRowSound);
				}

				else
				{
					Field[WIDTHCELLS * Row + Col]->SetRelativeScale3D(FVector(Cf));
				}
			}
		}
	}
	
	else if (Count == 6)
	{
		int32 RowNumber = 1;
		
		for (int32 Row = FullRows[0] + 1; Row < HEIGHTCELLS; ++Row)
		{
			int32 NullCount = 0;
			
			for (int32 Col = 0; Col < WIDTHCELLS; ++Col)
			{
				if (Field[WIDTHCELLS * Row + Col] != nullptr)
				{
					FVector CurrentLocation = Field[WIDTHCELLS * Row + Col]->GetRelativeLocation();
					Field[WIDTHCELLS * Row + Col]->SetRelativeLocation(FVector(CurrentLocation.X, CurrentLocation.Y, 100 * (Row - RowNumber)));
					Field[WIDTHCELLS * (Row - RowNumber) + Col] = Field[WIDTHCELLS * Row + Col];
					Field[WIDTHCELLS * Row + Col] = nullptr;
				}
				else
				{
					++NullCount;
				}
			}
			
			if (NullCount == WIDTHCELLS)
			{
				++RowNumber;
			}
		}
		
		bIsGameInProgress = true;
		GetWorld()->GetTimerManager().PauseTimer(ClearRowTimerHandle);
		
		GetWorldTimerManager().SetTimer(MoveVerticalTimerHandle,
									this,
									&AUT_MainField::MoveVerticalByTimer,
									Speed[CurrentSpeed],
									true,
									0.f);
	}
}

/** ------------------------------ CHECKS ---------------------------------------------- **/

bool AUT_MainField::CheckNewFigurePosition()
{
	CYCLEFOR(4)
	{
		if (CurrentFigure[i]->CurrentCoords.Y < HEIGHTCELLS &&
			Field[WIDTHCELLS * CurrentFigure[i]->CurrentCoords.Y + CurrentFigure[i]->CurrentCoords.X] != nullptr)
		{
			return true;
		}
	}
	
	return false;
}

bool AUT_MainField::CheckCurrentFigurePosition()
{
	CYCLEFOR(4)
	{
		if 	(CurrentFigure[i]->CurrentCoords.Y < 0 ||
			CurrentFigure[i]->CurrentCoords.X < 0 ||
			CurrentFigure[i]->CurrentCoords.X >= WIDTHCELLS ||
			(CurrentFigure[i]->CurrentCoords.Y < HEIGHTCELLS &&
			Field[WIDTHCELLS * CurrentFigure[i]->CurrentCoords.Y + CurrentFigure[i]->CurrentCoords.X] != nullptr))
		{
			return true;
		}
	}
	
	return false;
}

int32 AUT_MainField::GetMostLeftBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int32 MostLeftBrick = 0;
	
	for (int32 i = 1; i < 4; ++i)
	{
		if (Figure[MostLeftBrick]->CurrentCoords.X > Figure[i]->CurrentCoords.X)
		{
			MostLeftBrick = i;
		}	
	}
	return MostLeftBrick;
}

int32 AUT_MainField::GetMostBottomBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int32 MostBottomBrick = 0;
	
	for (int32 i = 1; i < 4; ++i)
	{
		if (Figure[MostBottomBrick]->CurrentCoords.Y > Figure[i]->CurrentCoords.Y)
		{
			MostBottomBrick = i;
		}
	}
	
	return MostBottomBrick;
}

int32 AUT_MainField::GetMostRightBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int32 MostRightBrick = 0;
	
	for (int32 i = 1; i < 4; ++i)
	{
		if (Figure[MostRightBrick]->CurrentCoords.X < Figure[i]->CurrentCoords.X)
		{
			MostRightBrick = i;
		}
	}
	
	return MostRightBrick;
}

int32 AUT_MainField::GetMostTopBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int32 MostTopBrick = 0;
	
	for (int32 i = 1; i < 4; ++i)
	{
		if (Figure[MostTopBrick]->CurrentCoords.Y < Figure[i]->CurrentCoords.Y)
		{
			MostTopBrick = i;
		}
	}
	
	return MostTopBrick;
}

void AUT_MainField::CheckRow()
{
	if (!bIsGameInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::CheckRow --- Game is not in progress"));
		return;
	}
	
	FullRows.Empty();
	
	for (int32 Row = 0, Col; Row < HEIGHTCELLS; ++Row)
	{
		for (Col = 0; Col < WIDTHCELLS; ++Col)
		{
			if (Field[WIDTHCELLS * Row + Col] == nullptr)
			{
				break;
			}
		}
		
		if (Col == WIDTHCELLS)
		{
			FullRows.Add(Row);
		}
	}
	
	if (FullRows.Num() > 0)
	{
		bIsGameInProgress = false;
		GetWorld()->GetTimerManager().SetTimer(
									ClearRowTimerHandle,
									this,
									&AUT_MainField::DeleteRow,
									0.1f,
									true,
									0.f);
	}
}


/** ------------------------------ MOVEMENT ---------------------------------------------- **/

void AUT_MainField::MoveVerticalCurrentFigure(int Amount, bool bBottom, bool bSaveCurrentPosition)
{
	CYCLEFOR(4)
	{
		CurrentFigure[i]->SetPoint(FVector2D(0, Amount * (bBottom ? -1 : 1)), bSaveCurrentPosition);
	}
}

void AUT_MainField::MoveHorizontalCurrentFigure(int Amount, bool bRight, bool bSaveCurrentPosition)
{
	CYCLEFOR(4)
	{
		CurrentFigure[i]->SetPoint(FVector2D(Amount * (bRight ? 1 : -1), 0), bSaveCurrentPosition);
	}
}

void AUT_MainField::MoveVerticalByTimer()
{
	if (!bIsGameInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::MoveDownByTimer --- Game is not in progress"));
		return;
	}
	
	MoveVerticalCurrentFigure(1, true, true);
	
	if (CheckCurrentFigurePosition())
	{
		RestorePoint();
		ReleaseFigure();
	}
	else
	{
		ApplyFigurePoint(CurrentFigure);
	}
}

void AUT_MainField::MoveHorizontalByTimer()
{
	if (!bIsGameInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::MoveRightByTimer --- Game is not i prigress"));
		return;
	}
	
	MoveHorizontalCurrentFigure(1, true, true);
	if (CheckCurrentFigurePosition())
	{
		RestorePoint();
		ReleaseFigure();
	}
	else
	{
		ApplyFigurePoint(CurrentFigure);
	}
}

void AUT_MainField::RotateFigure(TArray<TObjectPtr<UUT_Brick>>& Figure, bool bRight, bool bSaveCurrentPosition)
{
	CYCLEFOR(4)
	{
		if (i == 1) continue;
		
		Figure[i]->RotateBrick(FVector2D(Figure[1]->CurrentCoords.X, Figure[1]->CurrentCoords.Y), bRight, bSaveCurrentPosition);
	}
}