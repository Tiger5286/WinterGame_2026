#include "Physics.h"
#include <algorithm>
#include <cassert>
#include "Transform.h"

void Physics::Init(Transform* pTransform)
{
	if (pTransform == nullptr)
	{
		assert(false && "Physics::Init() : pTransform‚ªnullptr‚Å‚·");
	}

	m_pTransform = pTransform;
}

void Physics::Update()
{
	m_vel += m_accel;
	m_vel.y += m_gravity;

	m_vel.x *= m_drag;
	m_vel.z *= m_drag;

	m_pTransform->pos += m_vel;
}

void Physics::SetDrag(float drag)
{
	m_drag = std::clamp(drag, 0.0f, 1.0f);
}