// Fill out your copyright notice in the Description page of Project Settings.

#include "MMOInput.h"
#include "SkillJoystickPlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Components/DecalComponent.h"
#include "Components/TextBlock.h"

UMMOInput::UMMOInput()
{
	ArrowComponent = NULL;
	RangeOutComponent = NULL;
	RangeInComponent = NULL;

	TArray<FSimpleKeyState> States;
	States.Init(FSimpleKeyState(), EKeys::NUM_TOUCH_KEYS);

	KeyStateMap.Add(EGameKey::Tap, States);
	KeyStateMap.Add(EGameKey::Hold, States);
}

void UMMOInput::UpdateDetection(float DeltaTime)
{
	UpdateGameKeys(DeltaTime);
	ProcessKeyStates(DeltaTime);
}

void UMMOInput::AddWheelButton(WheelWidget& Button)
{
	WidgetList.Add(Button);
}

const TArray<WheelWidget>* UMMOInput::GetWheelWidgetList()
{
	return &WidgetList;
}

FVector2D UMMOInput::GetCurrRangeCenter()
{
	return CurrRangeCenter;
}

void UMMOInput::UpdateGameKeys(float DeltaTime)
{
	ASkillJoystickPlayerController* MyController = CastChecked<ASkillJoystickPlayerController>(GetOuter());

	// gather current states
	uint32 CurrentTouchState = 0;
	for (int32 i = 0; i < ARRAY_COUNT(MyController->PlayerInput->Touches); i++)
	{
		if (MyController->PlayerInput->Touches[i].Z != 0)
		{
			CurrentTouchState |= (1 << i);
		}
	}

	// detection
	FVector2D LocalPosition1 = FVector2D(MyController->PlayerInput->Touches[0]);
	//FVector2D LocalPosition2 = FVector2D(MyController->PlayerInput->Touches[1]);

	DetectPointActions(CurrentTouchState, PrevTouchState, DeltaTime, LocalPosition1, TouchAnchor, TouchDownTimeArray);

	// save states
	PrevTouchState = CurrentTouchState;
}

void UMMOInput::ProcessKeyStates(float DeltaTime)
{
	for (const FActionBinding& AB : ActionBindings)
	{
		const TArray<FSimpleKeyState>* KeyStates = KeyStateMap.Find(AB.Key);
		for (const FSimpleKeyState& State : *KeyStates)
		{
			if (State.Events[AB.KeyEvent] > 0)
			{
				AB.ActionDelegate.ExecuteIfBound(State.Position, State.DownTime);
			}
		}
	}

	// update states
	for (TMap<EGameKey::Type, TArray<FSimpleKeyState>>::TIterator It(KeyStateMap); It; ++It)
	{
		TArray<FSimpleKeyState>& KeyStates = It.Value();

		for (int i = 0; i < EKeys::NUM_TOUCH_KEYS; i++)
		{
			FSimpleKeyState& KeyState = KeyStates[i];
			if (KeyState.Events[IE_Pressed])
			{
				KeyState.bDown = true;

				if (EGameKey::Hold == It.Key())
				{
					//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Green, FString("AAAAAAAAAAAAAAAA "));
					UpdateWheelWidgetState(i, EWheelInput::Pressed);
				}
			}
			else if (KeyState.Events[IE_Released])
			{
				KeyState.bDown = false;

				if (EGameKey::Hold == It.Key())
				{
					//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Green, FString("BBBBBBBBBBB"));
					UpdateWheelWidgetState(i, EWheelInput::Released);
				}
			}

			FMemory::Memzero(KeyState.Events, sizeof(KeyState.Events));
		}
	}
}

void UMMOInput::DetectPointActions(uint32 CurrentTouchState, uint32 iPrevTouchState, float DeltaTime, const FVector2D& CurrentPosition, FVector2D& AnchorPosition, TArray<float>& DownTimeArray)
{
	const float HoldTime = 0.0f;

	if (CurrentTouchState ^ PrevTouchState)
	{
		TArray<FSimpleKeyState>& TapStates = KeyStateMap.FindOrAdd(EGameKey::Tap);
		TArray<FSimpleKeyState>& HoldStates = KeyStateMap.FindOrAdd(EGameKey::Hold);

		//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Magenta, FString("KKKKKK"));

		GEngine->AddOnScreenDebugMessage(-1, 4.1f, FColor::Magenta, FString("CurrentT:") + FString::FromInt(CurrentTouchState));
		GEngine->AddOnScreenDebugMessage(-1, 4.1f, FColor::Magenta, FString("Prev:") + FString::FromInt(PrevTouchState));

		for (int i = EKeys::NUM_TOUCH_KEYS - 1; i >= 0; i--)
		{
			bool CurrTouched = ((1 << i) & CurrentTouchState) > 0;
			bool PrevTouched = ((1 << i) & iPrevTouchState) > 0;

			if (CurrTouched)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Magenta, FString("LLLLLL"));
				// hold detection
				if (HoldStates[i].DownTime + DeltaTime> HoldTime && HoldStates[i].DownTime <= HoldTime)
				{
					HoldStates[i].Events[IE_Pressed]++;
					HoldStates[i].Position = CurrentPosition;
					//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Cyan, FString("QQQQQQQQQQ"));
				}

				HoldStates[i].DownTime += DeltaTime;
			}
			else
			{
				if (HoldStates[i].DownTime > 0)
				{
					HoldStates[i].Events[IE_Released]++;
					HoldStates[i].Position = CurrentPosition;
					//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Cyan, FString("WWWWWWWW"));
				}

				HoldStates[i].DownTime = 0;
			}

			if (!PrevTouched && CurrTouched)
			{
				//TapStates[i].DownTime += DeltaTime;
				TapStates[i].Events[IE_Pressed]++;
				TapStates[i].Position = CurrentPosition;
				//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Red, FString("111111111"));
			}
			else if (PrevTouched && !CurrTouched)
			{
				//TapStates[i].DownTime = 0;
				TapStates[i].Events[IE_Released]++;
				TapStates[i].Position = CurrentPosition;
				//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Red, FString("2222222222"));
			}
		}
	}
}

void UMMOInput::UpdateWheelWidgetState(int TouchIndex, EWheelInput::Type InputType)
{
	//const int BTN_TOUCH_RANGE = 100;
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString("111111"));

	/*FSimpleKeyState& HoldState = KeyStateMap.FindOrAdd(EGameKey::Hold);
	if (HoldState.bDown)
	{*/
		if(ASkillJoystickPlayerController* MyController = Cast<ASkillJoystickPlayerController>(GetOuter()))
		{
			FVector2D TouchPos = FVector2D(MyController->PlayerInput->Touches[TouchIndex]);
			/*FVector2D CursorPos;
			GEngine->GameViewport->GetMousePosition(CursorPos);*/

			for (int i = 0; i < WidgetList.Num(); i++)
			{
				WheelWidget& Widget = WidgetList[i];

				Widget.PrevInputType = Widget.CurrInputType;

				Widget.CurrInputType = EWheelInput::Released;

				if (EWheelInput::Pressed == InputType)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Blue, FString("CCCCCCCCC"));

					FVector2D ScreenPos = GetScreenPosition(Widget);

					FVector2D BottomBtnCenter = FVector2D(ScreenPos.X + Widget.DefaultImage.Width / 2, ScreenPos.Y + Widget.DefaultImage.Height / 2);

					if (FVector2D::Distance(BottomBtnCenter, TouchPos) <= Widget.DefaultImage.Width / 2)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 3.1f, FColor::Red, FString("EEEEEEEEEEEE"));

						Widget.CurrInputType = EWheelInput::Pressed;
						Widget.TouchIndex = TouchIndex;

						CurrWidgetIndex = i;

						/*FVector2D CursorPos;
						GEngine->GameViewport->GetMousePosition(CursorPos);*/
						CurrRangeCenter = TouchPos;

						//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString("NNNNNN: ") + CurrRangeCenter.ToString() + FString(" ") + FString::FromInt((int)InputType));
					}
				}
			}

			if (EWheelInput::Released == InputType)
			{
				CurrWidgetIndex = -1;
			}
		}
		
	//}
}

void UMMOInput::DrawWidget(AHUD* HUD, APlayerController* PlayerController)
{
	if (!HUD || !PlayerController)
	{
		return;
	}

	for (const WheelWidget& Widget : WidgetList)
	{
		const MMOImage& ImgDefault = Widget.DefaultImage;
		const MMOImage& ImgTop = Widget.TopImage;
		const MMOImage& ImgRange = Widget.RangeImage;

		FVector2D ScreenPos = GetScreenPosition(Widget);
		FVector2D DefaultImgCenter = FVector2D(ScreenPos.X + ImgDefault.Width / 2, ScreenPos.Y + ImgDefault.Height / 2);

		HUD->DrawTexture(ImgDefault.Image, DefaultImgCenter.X - ImgDefault.Width / 2, DefaultImgCenter.Y - ImgDefault.Height / 2, ImgDefault.Width, ImgDefault.Height, 0.f, 0.f, 1, 1);

		FVector2D TopImgCenter = DefaultImgCenter;//FVector2D(ImgTop.Position.X + ImgTop.Width / 2, ImgTop.Position.Y + ImgTop.Height / 2);


		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString("AAAAAAAAAAAAAAAA "));

		//Widget处于按下状态时
		if (Widget.CurrInputType == EWheelInput::Pressed && Widget.TouchIndex != -1)
		{
			/*FVector2D CursorPos;
			GEngine->GameViewport->GetMousePosition(CursorPos);*/
			FVector2D TouchPos = FVector2D(PlayerController->PlayerInput->Touches[Widget.TouchIndex]);

			TopImgCenter = TouchPos;

			FVector2D RangeCenter = CurrRangeCenter;

			if (FVector2D::Distance(RangeCenter, TouchPos) > ImgRange.Width / 2)
			{
				FVector2D Dire = TouchPos - DefaultImgCenter;
				Dire.Normalize();
				TopImgCenter = RangeCenter + Dire * (ImgRange.Width / 2);
			}

			HUD->DrawTexture(ImgTop.Image, TopImgCenter.X - ImgTop.Width / 2, TopImgCenter.Y - ImgTop.Height / 2, ImgTop.Width, ImgTop.Height, 0.f, 0.f, 1, 1);

			HUD->DrawTexture(ImgRange.Image, RangeCenter.X - ImgRange.Width / 2, RangeCenter.Y - ImgRange.Height / 2, ImgRange.Width, ImgRange.Height, 0.f, 0.f, 1, 1);

			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString("BBBBBBBB"));

			if (ASkillJoystickPlayerController* Controller = Cast<ASkillJoystickPlayerController>(GetWorld()->GetFirstPlayerController()))
			{
				switch (Widget.EffectType)
				{
				case EWheelEffect::Arrow:
				{
					DrawWheelArrow(PlayerController->GetPawn(), RangeCenter, TouchPos, Widget.MaterialArrow);
					break;
				}
				case EWheelEffect::Range:
				{
					//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString("CCCCCCCCCCCC"));
					DrawWheelRange(PlayerController->GetPawn(), RangeCenter, TouchPos, ImgRange.Width / 2, Widget.WorldRange, Widget.MaterialRangeIn, Widget.MaterialRangeOut);
					break;
				}
				}
			}
		}
		else if (Widget.CurrInputType == EWheelInput::Released && Widget.PrevInputType == EWheelInput::Pressed)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Black, FString("DDDDDDDDDDDDD"));
			//Widget处于按下释放状态时
			if (ArrowComponent)
			{
				ArrowComponent->DestroyComponent();
				ArrowComponent = NULL;
			}

			if (RangeOutComponent)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Emerald, FString("EEEEEEEEEEEEE"));
				RangeOutComponent->DestroyComponent();
				RangeOutComponent = NULL;
			}

			if (RangeInComponent)
			{
				RangeInComponent->DestroyComponent();
				RangeInComponent = NULL;
			}
		}
	}
}

void UMMOInput::DrawWheelArrow(AActor* Actor, const FVector2D& Center, const FVector2D& Dest, UMaterial* MaterialArrow)
{
	const float DECAL_SIZE = 300.f;
	if (!ArrowComponent)
	{
		ArrowComponent = NewObject<UDecalComponent>(Actor->GetRootComponent());
		if (ArrowComponent)
		{
			ArrowComponent->RegisterComponent();
			ArrowComponent->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			if (MaterialArrow)
			{
				ArrowComponent->SetDecalMaterial(MaterialArrow);
				ArrowComponent->DecalSize = FVector(DECAL_SIZE, DECAL_SIZE, DECAL_SIZE);
			}
		}
	}

	if (ArrowComponent)
	{
		FVector2D Dire2D = Dest - Center;
		FRotator Offset(0.f, 90.f, 0.f);

		if (FVector2D::ZeroVector == Dire2D)
		{
			FVector InitDire = Actor->GetActorRotation().Vector();
			Dire2D = FVector2D(InitDire.X, InitDire.Y);

			Offset = FRotator(0.f, 0.f, 0.f);
		}

		Dire2D.Normalize();

		FVector Dire3D(Dire2D.X, Dire2D.Y, 0.f);

		FVector DireWorld = Offset.RotateVector(Dire3D);

		FVector Loc = Actor->GetActorLocation() + DireWorld * DECAL_SIZE;
		Loc.Z = DECAL_SIZE - 20;

		ArrowComponent->SetWorldLocation(Loc);

		/*FRotator Rot = DireWorld.Rotation();
		FVector Vec(Dire2D.X, Dire2D.Y, 0.f);
		GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, FString("Dire2D: ") + Vec.Rotation().ToString() + FString(" ") + FString("Rot: ") + DireWorld.Rotation().ToString());*/

		FRotator Rot2 = (Loc - Actor->GetActorLocation()).Rotation();
		ArrowComponent->SetWorldRotation(FRotator(-90.f, Rot2.Yaw, 0.f));
	}
}

void UMMOInput::DrawWheelRange(AActor* Actor, const FVector2D& Center, const FVector2D& Dest, float ScreenMoveRange, float WorldMoveRange, UMaterial* MaterialRangeIn, UMaterial* MaterialRangeOut)
{
	const float DECAL_SIZE = 100.f;

	if (!RangeInComponent)
	{
		RangeInComponent = NewObject<UDecalComponent>(Actor->GetRootComponent());
		if (RangeInComponent)
		{
			RangeInComponent->RegisterComponent();
			RangeInComponent->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			
			if (MaterialRangeIn)
			{
				RangeInComponent->SetDecalMaterial(MaterialRangeIn);
				RangeInComponent->DecalSize = FVector(DECAL_SIZE + 100, DECAL_SIZE, DECAL_SIZE);

				RangeInComponent->SetWorldRotation(FRotator(-90.f, 0, 0.f));
			}
		}
	}

	if (!RangeOutComponent)
	{
		RangeOutComponent = NewObject<UDecalComponent>(Actor->GetRootComponent());
		if (RangeOutComponent)
		{
			RangeOutComponent->RegisterComponent();
			RangeOutComponent->AttachToComponent(Actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			
			if (MaterialRangeOut)
			{
				RangeOutComponent->SetDecalMaterial(MaterialRangeOut);
				RangeOutComponent->DecalSize = FVector(WorldMoveRange + 100, WorldMoveRange, WorldMoveRange);

				RangeOutComponent->SetWorldRotation(FRotator(-90.f, 0, 0.f));
				RangeOutComponent->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
			}
		}
	}

	if (RangeInComponent)
	{
		FVector2D Dire2D = Dest - Center;
		FRotator Offset(0.f, 90.f, 0.f);

		Dire2D.Normalize();

		FVector Dire3D(Dire2D.X, Dire2D.Y, 0.f);

		FVector DireWorld = Offset.RotateVector(Dire3D);

		float ScreenMoveLen = FVector2D::Distance(Dest, Center);
		ScreenMoveLen = ScreenMoveLen > ScreenMoveRange ? ScreenMoveRange : ScreenMoveLen;
		FVector Loc = Actor->GetActorLocation() + DireWorld * (WorldMoveRange / ScreenMoveRange * ScreenMoveLen);
		Loc.Z = Actor->GetActorLocation().Z;

		RangeInComponent->SetWorldLocation(Loc);
	}
}

FVector2D UMMOInput::GetScreenPosition(const WheelWidget& Widget)
{
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	FVector2D Pos = Widget.Position;

	switch (Widget.AnchorType)
	{
	case EWheelAnchor::TopLeft:
	{
		break;
	}
	case EWheelAnchor::TopRight:
	{
		Pos.X = ViewportSize.X + Pos.X;
		break;
	}
	case EWheelAnchor::DownLeft:
	{
		Pos.Y = ViewportSize.Y + Pos.Y;
		break;
	}
	case EWheelAnchor::DownRight:
	{
		Pos.X = ViewportSize.X + Pos.X;
		Pos.Y = ViewportSize.Y + Pos.Y;
		break;
	}
	case EWheelAnchor::TopMid:
	{
		Pos.X = ViewportCenter.X + Pos.X;
		break;
	}
	case EWheelAnchor::DownMid:
	{
		Pos.X = ViewportCenter.X + Pos.X;
		Pos.Y = ViewportSize.Y + Pos.X;
		break;
	}
	case EWheelAnchor::LeftMid:
	{
		Pos.Y = ViewportCenter.Y + Pos.Y;
		break;
	}
	case EWheelAnchor::RightMid:
	{
		Pos.Y = ViewportSize.Y + Pos.Y;
		break;
	}
	}

	return Pos;
}
