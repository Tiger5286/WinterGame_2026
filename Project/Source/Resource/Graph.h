#pragma once
#include "Resource.h"
#include "DxLib.h"

class Graph : public Resource
{
public:
	Graph(int handle) : Resource(handle)
	{
	}

	~Graph()
	{
		DeleteGraph(m_handle.Get());
	}
};