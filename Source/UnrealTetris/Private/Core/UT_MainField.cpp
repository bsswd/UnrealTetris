// Unreal Tetris Game. Made by Alex Sinkin. (c)

#include "UnrealTetris/Public/Core/UT_MainField.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/UT_Brick.h"
#include "UObject/ConstructorHelpers.h"

#define CYCLEFOR(I) for (int i = 0; i < I; i++)

int FigureArr[7][4] = 
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
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(MainScene);	
	Camera->SetRelativeLocation(FVector(500.f, 25000.f, 900.f));
	Camera->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Camera->SetFieldOfView(10.f);
	
	MainScene = CreateDefaultSubobject<USceneComponent>("MainScene");
	RootComponent = MainScene;
	
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
	
	bIsGameInProgress = false;
}


void AUT_MainField::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorLocation(FVector(0.f));
	SetActorRotation(FRotator(0.f));
	
	ClearField();
	
	CreateNextFigure();
	StartFigure();
	CreateNextFigure();
		
	bIsGameInProgress = true;
	
	GetWorldTimerManager().SetTimer(MoveDownTimerHandle,
									this,
									&AUT_MainField::MoveDownByTimer,
									1.f,
									true,
									1.f);
}

void AUT_MainField::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AUT_MainField::ClearField()
{	
	for (int i = 0; i < WIDTHCELLS * HEIGHTCELLS; i++)
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
	
	for (int i = 0; i < WIDTHCELLS * HEIGHTCELLS; i++)
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

void AUT_MainField::CreateNextFigure()
{
	int NextElement = FMath::Rand() % 7;
	
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
		//NextFigure[i]->Brick->SetRelativeScale3D(FVector(0.9f));
		
		//NextFigure[i]->ApplyPoint();
	}
	
	int NextRotation = FMath::Rand() % 6;
	
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
		GetWorld()->GetTimerManager().PauseTimer(MoveDownTimerHandle);
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

void AUT_MainField::RotateFigure(TArray<TObjectPtr<UUT_Brick>>& Figure, bool bRight, bool bSaveCurrentPosition)
{
	CYCLEFOR(4)
	{
		if (i == 1) continue;
		
		Figure[i]->RotateBrick(FVector2D(Figure[1]->CurrentCoords.X, Figure[1]->CurrentCoords.Y), bRight, bSaveCurrentPosition);
	}
}

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

void AUT_MainField::MoveVerticalCurrentFigure(int Amount, bool bBottom, bool bSaveCurrentPosition)
{
	CYCLEFOR(4)
	{
		CurrentFigure[i]->SetPoint(FVector2D(0, Amount * (bBottom ? -1 : 1)), bSaveCurrentPosition);
	}
}

void AUT_MainField::MoveHorizontalCurrentFigure(int Amount, bool bRight, bool bSaveCurrentPosition)
{
}

int AUT_MainField::GetMostLeftBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int MostLeftBrick = 0;
	
	for (int i = 1; i < 4; ++i)
	{
		if (Figure[MostLeftBrick]->CurrentCoords.X > Figure[i]->CurrentCoords.X)
		{
			MostLeftBrick = i;
		}	
	}
	return MostLeftBrick;
}

int AUT_MainField::GetMostBottomBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int MostBottomBrick = 0;
	
	for (int i = 1; i < 4; ++i)
	{
		if (Figure[MostBottomBrick]->CurrentCoords.Y > Figure[i]->CurrentCoords.Y)
		{
			MostBottomBrick = i;
		}
	}
	
	return MostBottomBrick;
}

int AUT_MainField::GetMostRightBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int MostRightBrick = 0;
	
	for (int i = 1; i < 4; ++i)
	{
		if (Figure[MostRightBrick]->CurrentCoords.X < Figure[i]->CurrentCoords.X)
		{
			MostRightBrick = i;
		}
	}
	
	return MostRightBrick;
}

int AUT_MainField::GetMostTopBrick(TArray<TObjectPtr<UUT_Brick>>& Figure)
{
	int MostTopBrick = 0;
	
	for (int i = 1; i < 4; ++i)
	{
		if (Figure[MostTopBrick]->CurrentCoords.Y < Figure[i]->CurrentCoords.Y)
		{
			MostTopBrick = i;
		}
	}
	
	return MostTopBrick;
}

void AUT_MainField::MoveDownByTimer()
{
	if (!bIsGameInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUT_MainField::MoveDownByTimer --- Game is not i prigress"));
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

void AUT_MainField::ReleaseFigure()
{
	if (CurrentFigure[GetMostTopBrick(CurrentFigure)]->CurrentCoords.Y >= HEIGHTCELLS)
	{
		bIsGameInProgress = false;
		GetWorldTimerManager().PauseTimer(MoveDownTimerHandle);
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
}
