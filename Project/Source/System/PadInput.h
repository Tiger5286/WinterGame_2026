#pragma once
#include "DxLib.h"
#include "Utility/Vector2.h"

class PadInput
{
public:
	static PadInput& GetInstance();
	virtual ~PadInput() = default;

	enum class LR
	{
		Left,
		Right
	};

	enum DPad
	{
		Left,
		Right,
		Up,
		Down
	};

private:
	PadInput(const PadInput&) = delete;
	PadInput& operator=(const PadInput&) = delete;
	PadInput() = default;
public:

	void Update();

	bool IsPressed(int xInput);
	bool IsTriggerd(int xInput, bool isMargeStickAndDPad);
	bool IsReleased(int xInput);

	Vector2 GetStickInput(LR lr);

	bool IsPressedTrigger(LR lr);
	bool IsTriggeredTrigger(LR lr);
	bool IsReleasedTrigger(LR lr);

private:
	Vector2 ConvertStickInput(const Vector2& stick);
	bool IsPrevPressedTrigger(LR lr);

private:
	XINPUT_STATE m_nowInput = {};
	XINPUT_STATE m_prevInput = {};
};

