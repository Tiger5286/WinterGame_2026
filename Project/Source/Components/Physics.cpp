#include "Physics.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include "Transform.h"
#include "Utility/Vector2.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Components/Collider/Collider.h"

namespace
{
	constexpr float kMaxFloorAngle = DX_PI_F / 4;
	// これより深い段差では吸着せず、通常の落下を行う。
	constexpr float kGroundSnapDistance = 12.0f;
	constexpr int kGroundSnapIterations = 12;
}

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
	m_isGrounded = false;
}

void Physics::Update()
{
	// ジャンプで上向きの速度を設定したフレームは吸着しない。
	const bool canSnap = m_isGrounded && m_vel.y <= 0.0f;
	m_isGrounded = false;
	const float minFloorNormalY = std::cos(kMaxFloorAngle);

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
	if (m_pCollider && m_pCollider->IsEnable())
	{
		auto& collisionManager = ServiceLocator::GetInstance().GetCollisionManager();
		CollisionManager::HitInfo hitResult = collisionManager.CheckCollision(*m_pCollider, movedPos);
		float floorPush = 0.0f;
		if (!hitResult.contactInfo.empty())
		{
			for (auto& contact : hitResult.contactInfo)
			{
				bool isFloor = false;
				// 当たった相手がポリゴンなら床壁判定をする
				if (contact.type == Collider::Type::Polygon)
				{
					isFloor = contact.normal.y > minFloorNormalY;
				}

				// 許容角度未満の面を床として扱う。
				if (isFloor)
				{
					// 同じ位置で得た補正を足すと三角形の境界で押し戻し過ぎるため、最大値を使う。
					floorPush = (std::max)(floorPush, contact.pushDist / contact.normal.y);
					if (m_vel.y <= 0.0f) m_isGrounded = true;
				}
				else
				{
					movedPos += contact.normal * contact.pushDist;
				}

				// 面に向かう速度成分を取り除く
				if (isFloor)
				{
					if (m_vel.y < 0.0f)
					{
						m_vel.y = 0.0f;
					}
				}
				else
				{
					const float normalSpeed = m_vel.Dot(contact.normal);
					if (normalSpeed < 0.0f)
					{
						m_vel -= contact.normal * normalSpeed;
					}
				}
			}
		}
		movedPos.y += floorPush;
	}

	// 補正済みの位置を反映する。
	m_pTransform->pos = movedPos;
}

void Physics::SetDrag(float drag)
{
	m_drag = std::clamp(drag, 0.0f, 1.0f);
}
