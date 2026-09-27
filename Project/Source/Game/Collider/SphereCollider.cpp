#include "SphereCollider.h"
#include "DxLib.h"
#include "Components/Transform.h"

#include <algorithm>

SphereCollider::SphereCollider(Transform& transform, float radius) : 
	Collider(Type::Sphere, transform),
	m_radius((std::max)(radius, 0.0f))
{
}

void SphereCollider::SetRadius(float radius)
{
	m_radius = (std::max)(radius, 0.0f);
}

void SphereCollider::Draw()
{
	if (!IsEnable())
	{
		return;
	}

	DrawSphere3D(m_transform.pos.ToDxLib(), m_radius, 16, 0x00ff00, 0xffffff, false);
}
