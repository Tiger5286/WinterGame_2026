#pragma once
#include <vector>
#include <memory>
#include "Components/Transform.h"

class GameObject
{
public:
	GameObject() = default;
	virtual ~GameObject() = default;

	// ‰Šú‰»ˆ—
	virtual void Init() abstract;
	// XVˆ—
	virtual void Update() abstract;
	// •`‰æˆ—
	virtual void Draw() const abstract;

private:
	Transform m_transform;
};