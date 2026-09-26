#include "Physics.h"
#include <algorithm>
#include <cassert>
#include "Transform.h"
#include "Utility/Vector2.h"

void Physics::Init(Transform* pTransform, float drag, float gravity)
{
	SetDrag(drag);
	m_gravity = gravity;

	if (pTransform == nullptr)
	{
		assert(false && "Physics::Init() : pTransformがnullptrです");
	}

	m_pTransform = pTransform;
}

void Physics::Update()
{
	// 速度に加速度を足す
	m_vel += m_accel;
	m_vel.y += m_gravity;

	// 水平移動は減衰する
	m_vel.x *= m_drag;
	m_vel.z *= m_drag;

	// 水平移動の最高速度を設定
	Vector2 velXZ = Vector2(m_vel.x, m_vel.z);
	if (velXZ.SquaredLength() > m_maxSpeed * m_maxSpeed)
	{
		m_vel.x = velXZ.x;
		m_vel.z = velXZ.y;
	}

	// 位置に速度を足す
	m_pTransform->pos += m_vel;
}

void Physics::SetDrag(float drag)
{
	m_drag = std::clamp(drag, 0.0f, 1.0f);
}