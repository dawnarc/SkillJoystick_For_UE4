// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SkillJoystickPlayerController.h"
#include "NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "SkillJoystickCharacter.h"
#include "MMOInput.h"

ASkillJoystickPlayerController::ASkillJoystickPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	InputHandler = NULL;
}

void ASkillJoystickPlayerController::OnTapPressed(const FVector2D& ScreenPosition, float DownTime)
{
	FString Str = FString::Printf(TEXT("Tap-Pressed:%s Time:%d"), *ScreenPosition.ToString(), DownTime);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, Str);
}

void ASkillJoystickPlayerController::OnHoldPressed(const FVector2D& ScreenPosition, float DownTime)
{
	FString Str = FString::Printf(TEXT("Hold-Pressed:%s Time:%d"), *ScreenPosition.ToString(), DownTime);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Str);
}

void ASkillJoystickPlayerController::OnHoldReleased(const FVector2D& ScreenPosition, float DownTime)
{
	FString Str = FString::Printf(TEXT("Hold-Released:%s Time:%d"), *ScreenPosition.ToString(), DownTime);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, Str);
}

void ASkillJoystickPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	/*if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}*/
}

void ASkillJoystickPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	//================== 技能摇杆 begin ======================
	InputHandler = NewObject<UMMOInput>(this);

	//BIND_ACTION(InputHandler, EGameKey::Tap, IE_Pressed, &ASkillJoystickPlayerController::OnTapPressed);

	//BIND_ACTION(InputHandler, EGameKey::Hold, IE_Pressed, &ASkillJoystickPlayerController::OnHoldPressed);
	//BIND_ACTION(InputHandler, EGameKey::Hold, IE_Released, &ASkillJoystickPlayerController::OnHoldReleased);

	TexDefault = LoadObject<UTexture>(NULL, TEXT("Texture2D'/Game/UI/Image/xuxiaozhandou_3.xuxiaozhandou_3'"));
	TexTop = LoadObject<UTexture>(NULL, TEXT("Texture2D'/Game/UI/Image/jinengdi_001.jinengdi_001'"));
	TexRange = LoadObject<UTexture>(NULL, TEXT("Texture2D'/Game/UI/Image/gongjidi_001.gongjidi_001'"));
	TexTest = LoadObject<UTexture>(NULL, TEXT("Texture2D'/Game/UI/Image/jinengkuang_001.jinengkuang_001'"));

	if (TexDefault && TexTop)
	{
		/*MMOImage ImgDefault(TexDefault);
		MMOImage ImgTop(TexTop);
		MMOImage ImgRange(TexRange);
		WheelWidget Widget(ImgDefault, ImgTop, ImgRange, FVector2D(-400.f, -400.f), EWheelEffect::Arrow, EWheelAnchor::DownRight);*/

		MatArrow = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/UI/Material/m_arrow_2.m_arrow_2'"));
		MatRangeIn = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/UI/Material/m_range_in.m_range_in'"));
		MatRangeOut = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/UI/Material/m_range_out.m_range_out'"));

		WheelWidget Widget1(TexDefault, TexTop, TexRange, FVector2D(-600.f, -300.f), EWheelEffect::Range, EWheelAnchor::DownRight, 500.f, NULL, MatRangeIn, MatRangeOut);
		WheelWidget Widget2(TexDefault, TexTop, TexRange, FVector2D(-300.f, -600.f), EWheelEffect::Arrow, EWheelAnchor::DownRight, 0.f, MatArrow);

		/*FScriptDelegate Del;
		Del.BindUFunction(this, "OnBtnChangeImgClick",);*/
		//Widget1.BindAction(EGameKey::Hold, IE_Pressed, this, &ASkillJoystickPlayerController::OnHoldPressed);

		BIND_ACTION(&Widget1, EGameKey::Hold, IE_Pressed, &ASkillJoystickPlayerController::OnHoldPressed);
		BIND_ACTION(&Widget2, EGameKey::Hold, IE_Released, &ASkillJoystickPlayerController::OnHoldReleased);

		/*FActionSignature Del;
		Del.BindUObject(this, &ASkillJoystickPlayerController::OnHoldReleased, );*/

		InputHandler->AddWheelButton(Widget1);
		InputHandler->AddWheelButton(Widget2);
	}

	//================== 技能摇杆 end ======================

	//InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASkillJoystickPlayerController::OnSetDestinationPressed);
	//InputComponent->BindAction("SetDestination", IE_Released, this, &ASkillJoystickPlayerController::OnSetDestinationReleased);

	InputComponent->BindAxis("MoveForward", this, &ASkillJoystickPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASkillJoystickPlayerController::MoveRight);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASkillJoystickPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASkillJoystickPlayerController::MoveToTouchLocation);
}

void ASkillJoystickPlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	/*if (!bGamePaused && PlayerInput && InputHandler && !bIgnoreInput)
	{
		InputHandler->UpdateDetection(DeltaTime);
	}*/

	Super::ProcessPlayerInput(DeltaTime, bGamePaused);

	if (PlayerInput && InputHandler && !bIgnoreInput)
	{
		InputHandler->UpdateDetection(DeltaTime);
	}
}

void ASkillJoystickPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*if (UClass* CreateRoleClass = LoadClass<UMyUserWidget>(NULL, TEXT("WidgetBlueprint'/Game/TopDownCPP/Blueprints/NewWidgetBlueprint.NewWidgetBlueprint_C'")))
	{
		MyWidget = CreateWidget<UMyUserWidget>(this, CreateRoleClass);
		if (MyWidget)
		{
			MyWidget->AddToViewport();
		}
	}*/

	GLog->Log("Does something");
	UE_LOG(LogTemp, Log, TEXT("%d"), 1111);
	UE_LOG(LogTemp, Warning, TEXT("%d"), 1111);
}

void ASkillJoystickPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ASkillJoystickPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ASkillJoystickPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetNavigationSystem(this);
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASkillJoystickPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ASkillJoystickPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ASkillJoystickPlayerController::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		if (APawn* Pawn = GetPawn())
		{
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			Pawn->AddMovementInput(Direction, Value);
		}
	}
}

void ASkillJoystickPlayerController::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		if (APawn* Pawn = GetPawn())
		{
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			Pawn->AddMovementInput(Direction, Value);
		}
	}
}

void ASkillJoystickPlayerController::LoadResource()
{
	
}
