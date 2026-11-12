#pragma once
#include "Resource.h"
#include "DxLib.h"

class Font : public Resource
{
public:
	Font(int handle) : Resource(handle)
	{
	}

	~Font()
	{
		DeleteFontToHandle(m_handle.Get());
	}
};