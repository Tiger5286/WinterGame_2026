#pragma once
#include "Resource.h"
#include "EffekseerForDXLib.h"

class Effect : public Resource
{
public:
	Effect(int handle) :
		Resource(handle)
	{
	}

	~Effect()
	{
		DeleteEffekseerEffect(m_handle.Get());
	}
};