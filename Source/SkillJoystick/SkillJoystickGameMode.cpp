// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SkillJoystickGameMode.h"
#include "SkillJoystickPlayerController.h"
#include "SkillJoystickCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyHUD.h"

ASkillJoystickGameMode::ASkillJoystickGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASkillJoystickPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}


	static ConstructorHelpers::FClassFinder<AMyHUD> MyHUDClass(TEXT("/Game/TopDownCPP/Blueprints/MyHUDBP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		HUDClass = MyHUDClass.Class;
	}
}
