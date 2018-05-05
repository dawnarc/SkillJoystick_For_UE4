// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UMMOInput;

/**
 * 
 */
UCLASS()
class SKILLJOYSTICK_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AMyHUD();

	virtual void DrawHUD() override;

protected:

	void DrawSkillJoyStick();

private:

	void LoadAssets();

private:

	UPROPERTY()
		UTexture * TexWheelBottom;

	UPROPERTY()
		UTexture * TexWheelTop;

	UMMOInput* InputHandler;

	APlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, Category = Decal)
		FVector DecalSize;

	UPROPERTY(EditDefaultsOnly, Category = Decal)
		FRotator DecalRelativeRotation;

	UPROPERTY(EditDefaultsOnly, Category = Decal)
		FVector DecalRelativeLocation;
};
