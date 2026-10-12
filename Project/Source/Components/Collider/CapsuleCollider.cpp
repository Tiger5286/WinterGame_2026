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

void CapsuleCollider::Draw(const unsigned int color)
{
	if (!IsEnable())
	{
		return;
	}

	const Vector3 bottom = m_transform.pos + Vector3::Up() * m_radius + m_offset;
	const Vector3 top = m_transform.pos + Vector3::Up() * m_height + Vector3::Down() * m_radius + m_offset;
	DrawCapsule3D(bottom, top, m_radius, 16, color, 0xffffff, false);
}
