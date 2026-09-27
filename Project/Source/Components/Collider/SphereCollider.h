#pragma once
#include "Collider.h"

/// <summary>
/// 球形の当たり判定
/// </summary>
class SphereCollider final : public Collider
{
public:
	SphereCollider(Transform& transform, float radius);

	float GetRadius() const { return m_radius; }
	void SetRadius(float radius);

	void Draw() override;

private:
	float m_radius = 0.0f;
};
