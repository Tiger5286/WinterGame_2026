#include "Physics.h"
#include <algorithm>
#include <cassert>
#include "Transform.h"
#include "Utility/Vector2.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Components/Collider/Collider.h"

void Physics::Init(Transform* pTransform, Collider* pCollider, float drag, float gravity)
{
	SetDrag(drag);
	m_gravity = gravity;

	if (pTransform == nullptr)
	{
		assert(false && "Physics::Init() : pTransformがnullptrです");
	}

	m_pTransform = pTransform;

	m_pCollider = pCollider;
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
		velXZ.Normalize();
		velXZ *= m_maxSpeed;
		m_vel.x = velXZ.x;
		m_vel.z = velXZ.y;
	}

	// 当たり判定と押し戻し
	// 当たり判定がなければ処理しない
	Vector3 movedPos = m_pTransform->pos + m_vel;
	if (m_pCollider)
	{
		CollisionManager::HitInfo hitResult = ServiceLocator::GetInstance().GetCollisionManager().CheckCollision(*m_pCollider, movedPos);
		if (hitResult.isHit)
		{
			for (auto& poly : hitResult.polyInfos)
			{
				movedPos += poly.normal * poly.pushDist;

				// 面に向かう速度成分を取り除く
				const float normalSpeed = m_vel.Dot(poly.normal);
				if (normalSpeed < 0.0f)
				{
					m_vel -= poly.normal * normalSpeed;
				}
			}
		}
	}

	// 位置に速度を足す
	m_pTransform->pos = movedPos;
}

void Physics::SetDrag(float drag)
{
	m_drag = std::clamp(drag, 0.0f, 1.0f);
}