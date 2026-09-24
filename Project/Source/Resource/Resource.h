#pragma once
#include "Handle.h"

class Resource
{
public:
	Resource(int handle) : m_handle(handle)
	{
	}

	virtual ~Resource() = default;

	const int GetHandle() const
	{
		m_handle.Get();
	}

protected:
	Handle m_handle;
};