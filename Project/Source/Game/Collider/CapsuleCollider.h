#pragma once

#include "Collider.h"

/// <summary>
/// カプセル形状の当たり判定
/// </summary>
class CapsuleCollider final : public Collider
{
public:
	// heightはカプセル全体の高さ
	CapsuleCollider(Transform& transform, float radius, float height);

	float GetRadius() const { return m_radius; }
	void SetRadius(float radius);

	float GetHeight() const { return m_height; }
	void SetHeight(float height);

	void Draw() override;

private:
	float m_radius = 0.0f;
	float m_height = 0.0f;
};
