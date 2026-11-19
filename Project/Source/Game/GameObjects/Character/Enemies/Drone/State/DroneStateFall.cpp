#include "DroneStateFall.h"
#include "../Drone.h"
#include "Components/Animator/SimpleAnimator.h"
#include "Components/Hackable.h"
#include "Components/Collider/CapsuleCollider.h"
#include "Components/Transform.h"
#include "System/ServiceLocator.h"
#include "System/EffectManager.h"
#include "Game/CollisionManager.h"
#include <algorithm>

namespace
{
	// 1フレームに増える落下速度
	constexpr float kGravity = 0.5f;
	// 落下速度の上限
	constexpr float kMaxFallSpeed = 30.0f;
	// 床が見つからなくても、この時間落ちたら爆発する(ステージの外に落ちた時用)
	constexpr int kMaxFallFrame = 300;
}

DroneStateFall::DroneStateFall(Drone& drone) :
	DroneState(drone)
{
}

DroneStateFall::~DroneStateFall()
{
}

void DroneStateFall::Enter()
{
	// 落下中はアニメーションを止める
	m_owner.GetComponent<SimpleAnimator>()->Pause(true);
	// コンポーネントは削除せず無効化する(削除すると参照している箇所が壊れるため)
	m_owner.GetComponent<Hackable>()->SetEnable(false);			// ハッキング対象から外す
	m_owner.GetComponent<CapsuleCollider>()->SetEnable(false);	// プレイヤーがすり抜けられるようにする
	m_fallSpeed = 0.0f;
	m_frame = 0;
}

void DroneStateFall::Update()
{
	float ts = m_owner.GetTimeScale();
	m_frame += ts;
	m_fallSpeed = (std::min)(m_fallSpeed + kGravity * ts, kMaxFallSpeed);

	auto transform = m_owner.GetComponent<Transform>();
	const Vector3 nextPos = transform->pos + Vector3::Down() * m_fallSpeed * ts;

	// 本体の中心から移動後の足元までの間に床があるか調べる
	const Vector3 center = transform->pos + Vector3::Up() * Drone::kBodyCenterHeight;
	auto result = ServiceLocator::GetInstance().GetCollisionManager().RayCast(center, nextPos, true);
	if (result.isHit)
	{
		// 床の位置に合わせてから爆発する
		transform->pos = result.hitPos;
		Explode();
		return;
	}

	transform->pos = nextPos;

	if (m_frame >= kMaxFallFrame)
	{
		Explode();
		return;
	}
}

void DroneStateFall::Exit()
{
}

void DroneStateFall::Explode()
{
	const Vector3 center = m_owner.GetComponent<Transform>()->pos + Vector3::Up() * Drone::kBodyCenterHeight;
	EffectManager::GetInstance().PlayEffect(L"Explosion", center);
	Die();
}
