// Unreal Tetris Game. Made by Alex Sinkin. (c)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "UT_MainField.generated.h"

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
	
	

protected:

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


};
