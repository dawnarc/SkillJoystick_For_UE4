// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "MMOInput.generated.h"

class AHUD;
class APlayerController;
class UDecalComponent;
class UMaterial;

DECLARE_DELEGATE_TwoParams(FActionSignature, const FVector2D&, float);

#define BIND_ACTION(Handler, ActionKey, ActionEvent, Delegate)	\
{ \
	if(Handler)	\
	{	\
		int32 Idx = (Handler)->ActionBindings.AddZeroed(); \
		(Handler)->ActionBindings[Idx].Key = ActionKey; \
		(Handler)->ActionBindings[Idx].KeyEvent = ActionEvent; \
		(Handler)->ActionBindings[Idx].ActionDelegate.BindUObject(this, Delegate); \
	}	\
}

namespace EGameKey
{
	enum Type
	{
		None,
		Tap,
		Hold,
		Swipe,
		SwipeTwoPoints,
		Pinch,
	};
}

//轮盘输入模式
namespace EWheelInput
{
	enum Type
	{
		Pressed,	//按住
		Released,	//释放
	};
}

//轮盘效果
namespace EWheelEffect
{
	enum Type
	{
		Arrow,	//箭头
		Range,	//范围
	};
}

namespace EWheelAnchor
{
	enum Type
	{
		TopMid,	//
		DownMid,	//
		LeftMid,	//
		RightMid,	//
		TopLeft,	//
		TopRight,	//
		DownLeft,	//
		DownRight,	//
	};
}


struct FActionBinding
{
	/** key to bind it to */
	EGameKey::Type Key;

	/** Key event to bind it to, e.g. pressed, released, dblclick */
	TEnumAsByte<EInputEvent> KeyEvent;

	/** action */
	FActionSignature ActionDelegate;
};

struct FSimpleKeyState
{
	/** current events indexed with: IE_Pressed, IE_Released, IE_Repeat */
	uint8 Events[3];

	/** is it pressed? (unused in tap & hold) */
	uint8 bDown : 1;

	/** position associated with event */
	FVector2D Position;

	/** position associated with event */
	FVector2D Position2;

	/** accumulated down time */
	float DownTime;

	FSimpleKeyState()
	{
		FMemory::Memzero(this, sizeof(FSimpleKeyState));
	}
};

struct MMOImage
{
	UTexture* Image;
	uint32 Width;
	uint32 Height;
	FVector2D PositionOffset;

	MMOImage()
	{

	}

	MMOImage(UTexture* Image, uint32 Width = 0, uint32 Height = 0, FVector2D PosOffset = FVector2D(0.f, 0.f))
	{
		if (Image)
		{
			this->Image = Image;
			this->Width = Width == 0 ? Image->GetSurfaceWidth() : Width;
			this->Height = Height == 0 ? Image->GetSurfaceHeight() : Height;
			this->PositionOffset = PosOffset;
		}
	}
};

struct WheelWidgetParameters
{
	UTexture* DefaultImg;
	UTexture* TopImg;
	UTexture* RangeImg;
	FVector2D WidgetPosition;
	EWheelEffect::Type EffectType;
	EWheelAnchor::Type AnchorType;
	float WorldRange;
	UMaterial* MaterialArrow;
	UMaterial* MaterialRangeIn;
	UMaterial* MaterialRangeOut;

	WheelWidgetParameters()
	{
		DefaultImg = NULL;
		TopImg = NULL;
		RangeImg = NULL;
		WidgetPosition = FVector2D::ZeroVector;
		EffectType = EWheelEffect::Arrow;
		AnchorType = EWheelAnchor::TopLeft;
		WorldRange = 0.f;
		MaterialArrow = NULL;
		MaterialRangeIn = NULL;
		MaterialRangeOut = NULL;
	}
};

struct WheelWidget
{
	WheelWidget()
	{
		DefaultImage = NULL;
		TopImage = NULL;
		RangeImage = NULL;
		Position = FVector2D::ZeroVector;
		TouchIndex = -1;
		EffectType = EWheelEffect::Arrow;
		AnchorType = EWheelAnchor::TopLeft;
		WorldRange = 0.f;
		MaterialArrow = NULL;
		MaterialRangeIn = NULL;
		MaterialRangeOut = NULL;
	}

	WheelWidget(MMOImage& DefaultImg, MMOImage& TopImg, MMOImage& RangeImg, FVector2D WidgetPosition, EWheelEffect::Type EffectType = EWheelEffect::Arrow, EWheelAnchor::Type AnchorType = EWheelAnchor::TopLeft, float WorldRange = 0.f, UMaterial* MaterialArrow = NULL, UMaterial* MaterialRangeIn = NULL, UMaterial* MaterialRangeOut = NULL)
	{
		DefaultImage = DefaultImg;
		TopImage = TopImg;
		RangeImage = RangeImg;
		Position = WidgetPosition;
		TouchIndex = -1;
		CurrInputType = EWheelInput::Released;
		this->EffectType = EffectType;
		this->AnchorType = AnchorType;
		this->WorldRange = WorldRange;
		this->MaterialArrow = MaterialArrow;
		this->MaterialRangeIn = MaterialRangeIn;
		this->MaterialRangeOut = MaterialRangeOut;
	}

	WheelWidget(UTexture* DefaultImg, UTexture* TopImg, UTexture* RangeImg, FVector2D WidgetPosition, EWheelEffect::Type EffectType = EWheelEffect::Arrow, EWheelAnchor::Type AnchorType = EWheelAnchor::TopLeft, float WorldRange = 0.f, UMaterial* MaterialArrow = NULL, UMaterial* MaterialRangeIn = NULL, UMaterial* MaterialRangeOut = NULL)
	{
		DefaultImage = DefaultImg;
		TopImage = TopImg;
		RangeImage = RangeImg;
		Position = WidgetPosition;
		TouchIndex = -1;
		CurrInputType = EWheelInput::Released;
		this->EffectType = EffectType;
		this->AnchorType = AnchorType;
		this->WorldRange = WorldRange;
		this->MaterialArrow = MaterialArrow;
		this->MaterialRangeIn = MaterialRangeIn;
		this->MaterialRangeOut = MaterialRangeOut;
	}

	WheelWidget(WheelWidgetParameters& Parameeters)
	{
		DefaultImage = Parameeters.DefaultImg;
		TopImage = Parameeters.TopImg;
		RangeImage = Parameeters.RangeImg;
		Position = Parameeters.WidgetPosition;
		TouchIndex = -1;
		CurrInputType = EWheelInput::Released;
		EffectType = Parameeters.EffectType;
		AnchorType = Parameeters.AnchorType;
		WorldRange = Parameeters.WorldRange;
		MaterialArrow = Parameeters.MaterialArrow;
		MaterialRangeIn = Parameeters.MaterialRangeIn;
		MaterialRangeOut = Parameeters.MaterialRangeOut;
	}

	/*void BindAction(EGameKey::Type Key, TEnumAsByte<EInputEvent>& KeyEvent, AActor* BindObj, FActionSignature ActionDelegate)
	{
		int32 Idx = ActionBindings.AddZeroed();
		ActionBindings[Idx].Key = Key;
		ActionBindings[Idx].KeyEvent = KeyEvent;
		ActionBindings[Idx].ActionDelegate.BindUObject(BindObj, ActionDelegate);
	}*/

	//默认按钮图片
	MMOImage DefaultImage;

	//顶部按钮图片
	MMOImage TopImage;

	//拖拽范围图片
	MMOImage RangeImage;

	//触屏坐标
	FVector2D Position;

	//触碰点索引
	int32 TouchIndex;

	//上一帧输入模式
	EWheelInput::Type PrevInputType;

	//当前输入模式
	EWheelInput::Type CurrInputType;

	//效果类型
	EWheelEffect::Type EffectType;

	//锚点类型
	EWheelAnchor::Type AnchorType;

	//EWheelEffect::Range类型widget在地图上的滑动范围
	float WorldRange;

	//Arrow类型技能的箭头材质
	UMaterial* MaterialArrow;

	//Range类型技能的范围材质，In为内部小圆材质，Out为范围限定材质
	UMaterial* MaterialRangeIn;
	UMaterial* MaterialRangeOut;

	//输入键绑定事件
	TArray<FActionBinding> ActionBindings;
};

/**
 * 
 */
UCLASS()
class SKILLJOYSTICK_API UMMOInput : public UObject
{
	GENERATED_BODY()
	
	
public:

	UMMOInput();

	TArray<FActionBinding> ActionBindings;
	
	/** update detection */
	void UpdateDetection(float DeltaTime);

	void AddWheelButton(WheelWidget& Button);

	const TArray<WheelWidget>* GetWheelWidgetList();

	FVector2D GetCurrRangeCenter();

	//回执UI。注意：只在HUD::DrawHUD()函数中调用
	void DrawWidget(AHUD* HUD, APlayerController* PlayerController);

protected:

	/** update game key recognition */
	void UpdateGameKeys(float DeltaTime);

	/** process input state and call handlers */
	void ProcessKeyStates(float DeltaTime);

	/** detect one point actions (touch and mouse) */
	void DetectPointActions(uint32 CurrentTouchState, uint32 iPrevTouchState, float DeltaTime, const FVector2D& CurrentPosition, FVector2D& AnchorPosition, TArray<float>& DownTimeArray);

	void UpdateWheelWidgetState(int TouchIndex, EWheelInput::Type InputType);

	void DrawWheelArrow(AActor* Actor, const FVector2D& Center, const FVector2D& Dest, UMaterial* MaterialArrow);

	void DrawWheelRange(AActor* Actor, const FVector2D& Center, const FVector2D& Dest, float ScreenMoveRange, float WorldMoveRange, UMaterial* MaterialRangeIn, UMaterial* MaterialRangeOut);

	FVector2D GetScreenPosition(const WheelWidget& Widget);

	//获取touch点的状态变化。CurrentTouchState：当前触点状态；CurrentTouchState：上一帧触点状态；Index：触点索引；Touched：是除按还是松开
	void GetTouchChangeInfo(uint32 CurrentTouchState, uint32 PrevTouchState, int32& Index, bool& Touched)
	{
		uint32 ChangeState = CurrentTouchState ^ PrevTouchState;
		if (ChangeState)
		{
			//检测当前帧的touch index变化状态。比如：touch index 2的状态从1变为了0。
			int Count = EKeys::NUM_TOUCH_KEYS;
			for (int i = Count - 1; i >= 0; i--)
			{
				//
				if ((1 << i) & ChangeState)
				{
					int TouchVal = 1 & (CurrentTouchState >> i);
					Touched = TouchVal == 1;
					Index = i;
					break;
				}
			}
		}
	}

protected:

	//上次按键的状态
	uint32 PrevTouchState;

	//Touch 按下时长
	TArray<float> TouchDownTimeArray;

	//触碰的屏幕坐标点
	FVector2D TouchAnchor;

	//按键状态map
	TMap<EGameKey::Type, TArray<FSimpleKeyState>> KeyStateMap;

	//widget列表
	TArray<WheelWidget> WidgetList;

	//当前正在使用的widget索引
	int CurrWidgetIndex;

	//当前按钮边界icon的中心位置坐标
	FVector2D CurrRangeCenter;

	UPROPERTY()
		UDecalComponent* ArrowComponent;

	UPROPERTY()
		UDecalComponent* RangeOutComponent;

	UPROPERTY()
		UDecalComponent* RangeInComponent;
};
