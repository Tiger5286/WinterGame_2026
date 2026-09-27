#include "CapsuleCollider.h"

#include "Components/Transform.h"
#include "DxLib.h"

#include <algorithm>

CapsuleCollider::CapsuleCollider(Transform& transform, float radius, float height) :
	Collider(Type::Capsule, transform),
	m_radius((std::max)(radius, 0.0f)),
	m_height((std::max)(height, m_radius * 2.0f))
{
}

void CapsuleCollider::SetRadius(float radius)
{
	m_radius = (std::max)(radius, 0.0f);
	m_height = (std::max)(m_height, m_radius * 2.0f);
}

void CapsuleCollider::SetHeight(float height)
{
	m_height = (std::max)(height, m_radius * 2.0f);
}

void CapsuleCollider::Draw()
{
	if (!IsEnable())
	{
		return;
	}

	const Vector3 bottom = m_transform.pos + Vector3::Up() * m_radius;
	const Vector3 top = m_transform.pos + Vector3::Up() * m_height + Vector3::Down() * m_radius;
	DrawCapsule3D(bottom.ToDxLib(), top.ToDxLib(), m_radius, 16, 0x00ff00, 0xffffff, false);
}
