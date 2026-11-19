#include "KeyboardInput.h"
#include "DxLib.h"

KeyboardInput& KeyboardInput::GetInstance()
{
	static KeyboardInput instance;
	return instance;
}

void KeyboardInput::Update()
{
	for (int i = 0; i < 256; i++)
	{
		m_prevKeyInput[i] = m_nowKeyInput[i];
	}

	GetHitKeyStateAll(m_nowKeyInput);
}

bool KeyboardInput::IsPressed(int keyInput) const
{
	return m_nowKeyInput[keyInput];
}

bool KeyboardInput::IsTriggerd(int keyInput) const
{
	return m_nowKeyInput[keyInput] && !m_prevKeyInput[keyInput];
}

bool KeyboardInput::IsReleased(int keyInput) const
{
	return !m_nowKeyInput[keyInput] && m_prevKeyInput[keyInput];
}