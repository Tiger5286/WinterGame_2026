#include "WalkerStateOrbit.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "../Walker.h"
#include "Components/Transform.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "WalkerStateApproach.h"
#include "Components/Animator/Animator.h"
#include "WalkerStateAttack.h"

namespace
{
	constexpr float kMaxMoveSpeed = 1.0f;
	constexpr float kChangeApproachDist = 130.0f;
}

WalkerStateOrbit::WalkerStateOrbit(Walker& walker) :
	WalkerState(walker)
{
}

WalkerStateOrbit::~WalkerStateOrbit()
{
}

void WalkerStateOrbit::Enter()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(kMaxMoveSpeed);
	m_owner.GetComponent<Animator>()->Play(L"Armature|LeftWalk");
}

void WalkerStateOrbit::Update()
{
	Vector3 playerPosXZ = GetPlayer()->GetComponent<Transform>()->pos;
	playerPosXZ.y = 0.0f;
	Vector3 thisPosXZ = m_owner.GetComponent<Transform>()->pos;
	thisPosXZ.y = 0.0f;

	Vector3 thisToPlayerXZ = playerPosXZ - thisPosXZ;

	if (thisToPlayerXZ.SquaredLength() > kChangeApproachDist * kChangeApproachDist)
	{
		m_owner.GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateApproach>(m_owner));
		return;
	}

	// プレイヤーとの水平距離
	const float distance = thisToPlayerXZ.Length();
	if (distance < 0.001f)
	{
		return; // 同じ位置では方向を計算できない
	}

	// プレイヤーへ向かう方向
	const Vector3 toPlayer = thisToPlayerXZ / distance;

	// 今と同じ周回方向
	const Vector3 tangent = toPlayer.Cross(Vector3::Up());

	constexpr float kOrbitDistance = 90.0f;   // 維持したい距離
	constexpr float kOrbitSpeed = 0.8f;       // 横移動の速度
	constexpr float kDistanceGain = 0.05f;    // 距離を戻す強さ
	constexpr float kMaxCorrectSpeed = 0.5f; // 距離補正の最高速度
	constexpr float kVelocityGain = 0.2f;    // 目標速度へ近づく強さ

	// 遠ければプレイヤーへ、近ければ離れる方向へ補正する
	float correctSpeed = (distance - kOrbitDistance) * kDistanceGain;

	if (correctSpeed > kMaxCorrectSpeed)
	{
		correctSpeed = kMaxCorrectSpeed;
	}
	else if (correctSpeed < -kMaxCorrectSpeed)
	{
		correctSpeed = -kMaxCorrectSpeed;
	}

	// 横移動と距離補正を合わせた「目標速度」
	const Vector3 targetVelocity =
		tangent * kOrbitSpeed + toPlayer * correctSpeed;

	auto physics = m_owner.GetComponent<Physics>();

	// 現在の水平速度を目標速度へ近づける
	const Vector3 currentVelocity(physics->m_vel.x, 0.0f, physics->m_vel.z);
	physics->m_accel = (targetVelocity - currentVelocity) * kVelocityGain;

	// 攻撃
	m_owner.GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateAttack>(m_owner));
	return;
}

void WalkerStateOrbit::Exit()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(Physics::kDefaultMaxSpeed);
}
