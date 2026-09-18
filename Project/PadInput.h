#pragma once
#include "DxLib.h"

class PadInput
{
public:
	PadInput() = default;
	~PadInput() = default;

	void Update();

private:
	XINPUT_STATE m_prevInput;
};

