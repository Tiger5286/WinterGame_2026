#pragma once
#include "Resource.h"
#include "DxLib.h"

class Model : public Resource
{
public:
	Model(int handle) : Resource(handle)
	{
	}

	~Model()
	{
		MV1DeleteModel(m_handle.Get());
	}

	void Draw() const
	{
		MV1DrawModel(m_handle.Get());
	}
};