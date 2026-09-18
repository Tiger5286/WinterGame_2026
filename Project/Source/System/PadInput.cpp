#include "PadInput.h"

namespace
{
	// スティック入力のデッドゾーン(-32768~32767)
	constexpr int kMinStickValue = 3000;
	constexpr int kMaxStickValue = 25000;
	// トリガーが反応するしきい値(0~255)
	constexpr int kTriggerThreshold = 128;
}

PadInput& PadInput::GetInstance()
{
	static PadInput instance;
	return instance;
}

void PadInput::Update()
{
	m_prevInput = m_nowInput;
	GetJoypadXInputState(DX_INPUT_PAD1, &m_nowInput);
}

bool PadInput::IsPressed(int xInput)
{
	return m_nowInput.Buttons[xInput];
}

bool PadInput::IsTriggerd(int xInput, bool isMargeStickAndDPad)
{
	if (isMargeStickAndDPad)
	{
		// 左スティック入力を十字キー入力に変換する
		if (GetStickInput(LR::Left).y > 0.5f) m_nowInput.Buttons[XINPUT_BUTTON_DPAD_UP] = true;
		if (GetStickInput(LR::Left).y < -0.5f) m_nowInput.Buttons[XINPUT_BUTTON_DPAD_DOWN] = true;
		if (GetStickInput(LR::Left).x > 0.5f) m_nowInput.Buttons[XINPUT_BUTTON_DPAD_RIGHT] = true;
		if (GetStickInput(LR::Left).x < -0.5f) m_nowInput.Buttons[XINPUT_BUTTON_DPAD_LEFT] = true;
	}
	return m_nowInput.Buttons[xInput] && !m_prevInput.Buttons[xInput];
}

bool PadInput::IsReleased(int xInput)
{
	return !m_nowInput.Buttons[xInput] && m_prevInput.Buttons[xInput];
}

Vector2 PadInput::GetStickInput(LR lr)
{
	Vector2 stick;
	if (lr == LR::Left)
	{
		stick.x = m_nowInput.ThumbLX;
		stick.y = m_nowInput.ThumbLY;
		stick = ConvertStickInput(stick);
	}
	else if (lr == LR::Right)
	{
		stick.x = m_nowInput.ThumbRX;
		stick.y = m_nowInput.ThumbRY;
		stick = ConvertStickInput(stick);
	}
	return stick;
}

bool PadInput::IsPressedTrigger(LR lr)
{
	// トリガー入力を取得
	unsigned char trigger = 0;
	if (lr == LR::Left)
	{
		trigger = m_nowInput.LeftTrigger;
	}
	else if (lr == LR::Right)
	{
		trigger = m_nowInput.RightTrigger;
	}
	// 一定以上押されていたら押した判定
	if (trigger > kTriggerThreshold)
	{
		return true;
	}
	return false;
}

bool PadInput::IsTriggeredTrigger(LR lr)
{
	return IsPressedTrigger(lr) && !IsPrevPressedTrigger(lr);
}

bool PadInput::IsReleasedTrigger(LR lr)
{
	return !IsPressedTrigger(lr) && IsPrevPressedTrigger(lr);
}

Vector2 PadInput::ConvertStickInput(const Vector2& stick)
{
	Vector2 result = stick;
	// スティック入力のデッドゾーンを設定
	// -3000~3000の範囲を0とする
	if (result.x > -kMinStickValue && result.x < kMinStickValue) result.x = 0.0f;
	if (result.y > -kMinStickValue && result.y < kMinStickValue) result.y = 0.0f;
	// 25000を超える入力は25000として扱う
	if (result.x > kMaxStickValue) result.x = kMaxStickValue;
	if (result.y > kMaxStickValue) result.y = kMaxStickValue;
	// -25000未満の入力は-25000として扱う
	if (result.x < -kMaxStickValue) result.x = -kMaxStickValue;
	if (result.y < -kMaxStickValue) result.y = -kMaxStickValue;

	// -25000~25000の範囲を-1.0~1.0の範囲に変換する
	result.x = result.x / kMaxStickValue;
	result.y = result.y / kMaxStickValue;

	// 長さが1を超えたら1にクランプする
	if (result.SquaredLength() > 1.0f) result.Normalize();

	return result;
}

bool PadInput::IsPrevPressedTrigger(LR lr)
{
	// トリガー入力を取得
	unsigned char trigger = 0;
	if (lr == LR::Left)
	{
		trigger = m_prevInput.LeftTrigger;
	}
	else if (lr == LR::Right)
	{
		trigger = m_prevInput.RightTrigger;
	}
	// 一定以上押されていたら押した判定
	if (trigger > kTriggerThreshold)
	{
		return true;
	}
	return false;
}
