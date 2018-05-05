// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SkillJoystickPlayerController.generated.h"

class UMMOInput;

UCLASS()
class ASkillJoystickPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASkillJoystickPlayerController();

	void OnTapPressed(const FVector2D& ScreenPosition, float DownTime);
	void OnHoldPressed(const FVector2D& ScreenPosition, float DownTime);
	void OnHoldReleased(const FVector2D& ScreenPosition, float DownTime);

	UMMOInput* GetInputHandler() {return InputHandler; }

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
	// End PlayerController interface

	virtual void BeginPlay() override;

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	void MoveForward(float Value);
	void MoveRight(float Value);

	void LoadResource();

protected:

	/** if set, input and camera updates will be ignored */
	uint8 bIgnoreInput : 1;

	UPROPERTY()
		UMMOInput* InputHandler;

	UPROPERTY()
	UTexture* TexDefault;

	UPROPERTY()
	UTexture* TexTop;

	UPROPERTY()
	UTexture* TexRange;

	UPROPERTY()
	UTexture* TexTest;

	UPROPERTY()
	UMaterial* MatArrow;

	UPROPERTY()
	UMaterial* MatRangeIn;

	UPROPERTY()
	UMaterial* MatRangeOut;
};


