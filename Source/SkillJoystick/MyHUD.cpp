// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"
#include "SkillJoystickPlayerController.h"
#include "MMOInput.h"
#include "Materials/Material.h"
#include "EngineUtils.h"
#include "Engine/DecalActor.h"

AMyHUD::AMyHUD()
{
	InputHandler = NULL;
	PlayerController = NULL;

	DecalSize = FVector(128.f, 256.f, 256.f);
	DecalRelativeRotation = FRotator(0.f, 0.f, 0.f); 
	DecalRelativeLocation = FVector(0.f, 0.f, -90.f);
}

void AMyHUD::DrawHUD()
{
	DrawSkillJoyStick();
}

void AMyHUD::DrawSkillJoyStick()
{
	if (!InputHandler)
	{
		if (ASkillJoystickPlayerController* Controller = Cast<ASkillJoystickPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PlayerController = Controller;
			InputHandler = Controller->GetInputHandler();
		}
	}
	else
	{
		InputHandler->DrawWidget(this, PlayerController);
	}
}

void AMyHUD::LoadAssets()
{
	
}
